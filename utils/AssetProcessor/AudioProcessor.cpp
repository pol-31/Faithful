#include "AudioProcessor.h"

#include <iostream>

#include "AssetLoadingThreadPool.h"

#include "../../config/AssetFormats.h"

void AudioProcessor::ThreadData::Init(int channels, int sample_rate) {
  vorbis_info_init(&vi);
  vorbis_comment_init(&vc);
  vorbis_encode_init_vbr(&vi, channels, sample_rate,
                         faithful::config::kAudioCompQuality);

  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);
  // don't need srand(time) because we simply concatenating audio
  ogg_stream_init(&os, 0);

  static bool headers_processed = false;

  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;

  vorbis_analysis_headerout(&vd, &vc, &header,
                            &header_comm, &header_code);
  ogg_stream_packetin(&os, &header);
  ogg_stream_packetin(&os, &header_comm);
  ogg_stream_packetin(&os, &header_code);

  if (!headers_processed) {
    std::ofstream out_file("audio_result.ogg", std::ios::binary);
    while (ogg_stream_flush(&os, &og)) {
      out_file.write(reinterpret_cast<const char*>(og.header),
                     og.header_len);
      out_file.write(reinterpret_cast<const char*>(og.body),
                     og.body_len);
    }
    headers_processed = true;
  }
}

AudioProcessor::ThreadData::~ThreadData() {
  ogg_stream_clear(&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  vorbis_comment_clear(&vc);
  vorbis_info_clear(&vi);
}

AudioProcessor::AudioProcessor(bool encode,
                               const std::filesystem::path& asset_destination,
                               const std::filesystem::path& user_asset_root_dir,
                               AssetLoadingThreadPool* thread_pool)
    : asset_destination_(asset_destination),
      user_asset_root_dir_(user_asset_root_dir),
      thread_pool_(thread_pool),
      encode_(encode){
  thread_number_ = thread_pool_->GetThreadNumber();
  thread_buffers_.resize(thread_number_);
  // thread_info_ resized for each new file
}

void AudioProcessor::Process(const std::filesystem::path& model_path,
                             AssetCategory category) {
  if (encode_) {
    switch (category) {
      case AssetCategory::kAudioMusic:
        EncodeMusic(model_path);
        break;
      case AssetCategory::kAudioSound:
        EncodeSound(model_path);
        break;
      default:
        std::cerr << "AudioProcessor::Process wrong category" << std::endl;
    }
  } else {
    std::terminate(); // NOT IMPLEMENTED
    // TODO: just copy *.ogg to destination
  }
}

void AudioProcessor::EncodeMusic(const std::filesystem::path& audio_path) {
  std::cout << "Music processing ..." << std::endl;
  size_t buffer_size = faithful::config::kAudioCompBufferSize * 4;
  auto pcm_data = std::make_unique<float[]>(buffer_size);
  uint64_t frames;

  InitThreadBuffers();
  PrepareCompressedOggDataFile("");

  if (audio_path.extension() == ".mp3") {
    drmp3 drmp3_context;
    if (!drmp3_init_file(&drmp3_context, audio_path.string().c_str(), nullptr)) {
      std::cerr << "Error: cannot open mp3 file: " << audio_path << std::endl;
      return;
    }
    PrepareThreadInfo(drmp3_context.channels, drmp3_context.sampleRate);
    while (true) {
      DecompressMp3Chunk(drmp3_context, pcm_data.get(), buffer_size, &frames);
      if (frames == 0) {
        break;
      }
      CompressChunk(pcm_data.get(), frames, drmp3_context.channels,
                    drmp3_context.sampleRate);
    }
    drmp3_uninit(&drmp3_context);
  } else if (audio_path.extension() == ".flac") {
    drflac* flac_context = drflac_open_file(audio_path.string().c_str(), nullptr);
    if (!flac_context) {
      std::cerr << "Error: cannot open flac file: " << audio_path << std::endl;
      return;
    }
    PrepareThreadInfo(flac_context->channels, flac_context->sampleRate);
    while (true) {
      DecompressFlacChunk(*flac_context, pcm_data.get(), buffer_size, &frames);
      if (frames == 0)
        break;
      CompressChunk(pcm_data.get(), frames, flac_context->channels,
                    flac_context->sampleRate);
    }
    drflac_close(flac_context);
  } else {  /// .ogg
    // TODO: if .ogg just copy to destination
    std::terminate(); // NOT IMPLEMENTED
  }
  cur_file_.close();
}


void AudioProcessor::PrepareThreadInfo(int channels, int sample_rate) {
  if (thread_info_.empty()) {
    thread_info_.resize(thread_number_);
    for (int i = 0; i < thread_info_.size(); ++i) {
      thread_info_[i].Init(channels, sample_rate);
    }
  }
}

void AudioProcessor::EncodeSound(const std::filesystem::path& audio_path) {
  // TODO: don't need buffer for Sound
  size_t buffer_size = faithful::config::kAudioCompBufferSize * 4;
  auto pcm_data = std::make_unique<float>(buffer_size);
  uint64_t frames;

  drwav drwav_context;
  if (!drwav_init_file(&drwav_context, audio_path.string().c_str(), nullptr)) {
    return;
  }

  int channels = drwav_context.channels;
  frames = 0;
  size_t framesRead;
  while ((framesRead = drwav_read_pcm_frames_f32(
              &drwav_context, faithful::config::kAudioCompBufferSize,
              pcm_data.get() + (frames * channels))) > 0) {
    frames += framesRead;
    if (buffer_size < (frames + 2 * faithful::config::kAudioCompBufferSize) * channels) {
      std::cerr << "AudioProcessor::EncodeSound buffer too small" << std::endl;
      break;
    }
  }
  CompressChunk(pcm_data.get(), frames, drwav_context.channels,
                drwav_context.sampleRate); // TODO: handle differently for SOund
  drwav_uninit(&drwav_context);
}

void AudioProcessor::DecompressMp3Chunk(drmp3& drmp3_context, float* pcm_data,
                                        std::size_t buffer_size, uint64_t* frames) {
  int channels = drmp3_context.channels;
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drmp3_read_pcm_frames_f32(
              &drmp3_context, faithful::config::kAudioCompBufferSize,
              pcm_data + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + faithful::config::kAudioCompBufferSize) * channels){
      std::cerr << buffer_size << " "
                << ((*frames + faithful::config::kAudioCompBufferSize) * channels)
                << std::endl;
      break;
    }
  }
  std::cerr << "DecompressMp3Chunk: " << *frames << std::endl;
}

void AudioProcessor::DecompressFlacChunk(drflac& drflac_context, float* pcm_data,
                                         std::size_t buffer_size, uint64_t* frames) {
  int channels = drflac_context.channels;
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drflac_read_pcm_frames_f32(
              &drflac_context, faithful::config::kAudioCompBufferSize,
              pcm_data + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * faithful::config::kAudioCompBufferSize)
                          * channels)
      break;
  }
}

void AudioProcessor::CompressChunk(
    float* pcm_data, uint64_t frames,
    int channels, int sample_rate) {
  int thread_offset = frames / thread_number_;

  int qwerty = faithful::config::kAudioCompChunkSize;

  PrepareOggCompressionContexts(channels, sample_rate, thread_offset);

  thread_pool_->Execute([&, qwerty, thread_offset](int thread_id) {
    ThreadData& context = thread_info_[thread_id];
    int cur_pos, next_pos;
    long int sentinel;
    if (thread_id != thread_number_ - 1) {
      next_pos = thread_id * thread_offset * channels;
    } else {
      next_pos = (thread_number_ - 1) * thread_offset * channels;
    }
    float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);
    while (true) {
      cur_pos = next_pos;
      next_pos += qwerty * channels;

      if (thread_id != thread_number_ - 1) {
        sentinel = (thread_id + 1) * thread_offset * channels - next_pos;
      } else {
        sentinel = frames * channels - next_pos;
      }

      if (sentinel >= 0) {
        sentinel = qwerty;
      } else {
        sentinel = qwerty + sentinel / channels;
      }

      for (int j = 0; j < channels; ++j) {
        for (long i = 0; i < sentinel; ++i) {
          buffer[j][i] = pcm_data[cur_pos + i * channels + j];
        }
      }
      vorbis_analysis_wrote(&context.vd, sentinel);

      while (vorbis_analysis_blockout(&context.vd, &context.vb) == 1) {
        vorbis_analysis(&context.vb, nullptr);
        vorbis_bitrate_addblock(&context.vb);  // Does we need it?
        vorbis_bitrate_flushpacket(&context.vd, &context.op);
        ogg_stream_packetin(&context.os, &context.op);
        while (ogg_stream_pageout(&context.os, &context.og)) {
          std::memcpy(reinterpret_cast<void*>(
                          thread_buffers_[thread_id].data.get()  + thread_buffers_[thread_id].size),
                      reinterpret_cast<void*>(context.og.header),
                      context.og.header_len);
          thread_buffers_[thread_id].size += context.og.header_len;
          std::memcpy(reinterpret_cast<void*>(
                          thread_buffers_[thread_id].data.get()  + thread_buffers_[thread_id].size),
                      reinterpret_cast<void*>(context.og.body),
                      context.og.body_len);
          thread_buffers_[thread_id].size += context.og.body_len;
        }
      }
      if (sentinel != qwerty) {
        break;
      }
    }
    std::cout << "TOTAL_" << thread_id << "_: " << context.op.granulepos
              << " " << context.op.packetno << std::endl;
  });

  WriteCompressedOggChunks(thread_offset);
}


void AudioProcessor::PrepareOggCompressionContexts(
    int channels, int sample_rate, int thread_offset) { // TODO: <----------------
  std::cout << "last granulepos: " << last_gramulepos_ << std::endl;
  for (auto& thread_buffer : thread_buffers_) {
    thread_buffer.size = 0;
  }
  for (int i = 0; i < thread_info_.size(); ++i) {
    thread_info_[i].vd.granulepos = last_gramulepos_ + thread_offset * i;
    thread_info_[i].vb.granulepos = last_gramulepos_ + thread_offset * i;
  }
}

void AudioProcessor::WriteCompressedOggChunks(int thread_offset) {
  last_gramulepos_ += thread_offset * thread_pool_->GetThreadNumber();
  for (const auto& b : thread_buffers_) {
    std::cout << "+" << b.size << std::endl;
    if (!cur_file_.is_open()) {
      std::terminate();
    }
    cur_file_.write(b.data.get(), b.size);
  }
}

void AudioProcessor::PrepareCompressedOggDataFile(
    const std::string& to_where) {
//  std::filesystem::remove(to_where);

  std::cout << std::filesystem::current_path() << std::endl;
  cur_file_.open("audio_result.ogg", std::ios::app | std::ios::binary);
  if (!cur_file_.is_open()) {
    std::cerr
        << "AudioProcessor::PrepareCompressedOggDataFile can't open file to write"
        << std::endl;
    std::terminate();
  }
}

void AudioProcessor::InitThreadBuffers() {
  if (!thread_buffers_initialized_) {
    for (auto& thread_buffer : thread_buffers_) {
      thread_buffer.data = std::make_unique<char[]>(faithful::config::kAudioCompBufferSize);
      thread_buffer.size = 0;
    }
    thread_buffers_initialized_ = true;
  }
}
