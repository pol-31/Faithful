#include "AudioProcessor.h"

#include <fstream>
#include <memory>

#include "AssetLoadingThreadPool.h"

#include "../../config/AssetFormats.h"


void AudioProcessor::ThreadData::Init(int channels, int sampleRate) {
  vorbis_info_init(&vi);
  vorbis_comment_init(&vc);
  vorbis_comment_add(&vc, "ARTIST=Faithful");
  vorbis_comment_add(&vc, "TITLE=Faithful");
  vorbis_encode_init_vbr(&vi, channels, sampleRate,
                         faithful::config::audio_comp_quality);

  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);
  srand(time(nullptr));
  ogg_stream_init(&os, rand());

  static bool headers_processed = false;

  if (!headers_processed) {
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header,
                              &header_comm, &header_code);
    ogg_stream_packetin(&os, &header);
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

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

AudioProcessor::AudioProcessor(const std::filesystem::path& asset_destination,
                               AssetLoadingThreadPool* thread_pool)
    : asset_destination_(asset_destination),
      thread_pool_(thread_pool),
      worker_threads_count_(thread_pool->get_thread_count()) {
  last_packetno_ = new std::vector<int>(thread_pool_->thread_count_ + 1);
  for (auto& p : *last_packetno_) {
    p = 0;
  }
}

AudioProcessor::~AudioProcessor() {
  delete last_packetno_;
}

void AudioProcessor::ProcessMusic(const std::filesystem::path& model_path,
                                  const std::filesystem::path& path_suffix) {
  std::cout << "Music processing ..." << std::endl;
  float* pPCM;
  uint64_t frames;

  for (auto& p : *last_packetno_) {
    p = 0;
  }

  int total_threads = worker_threads_count_ + 1;
  std::vector<std::pair<long int, std::unique_ptr<char>>> buffers(total_threads);
  for (int i = 0; i < total_threads; ++i) {
    buffers[i] = {0, std::unique_ptr<char>(
                              new char[faithful::config::audio_comp_buffer_size])};
  }
  if (model_path.extension() == ".mp3") {
    drmp3 drmp3_context;
    if (!drmp3_init_file(&drmp3_context, model_path.string().c_str(), nullptr)) {
      std::cerr << "Error: cannot open mp3 file: " << model_path << std::endl;
      return;
    }
    while (true) {
      DecompressMp3Chunk(drmp3_context, &pPCM, &frames);
      if (frames == 0) {
        break;
      }
      CompressChunk(model_path, pPCM, frames, drmp3_context.channels,
                    drmp3_context.sampleRate, buffers);
      delete[] pPCM;
    }
    drmp3_uninit(&drmp3_context);
  } else if (model_path.extension() == ".flac") {
    drflac* pFlac = drflac_open_file(model_path.string().c_str(), nullptr);
    if (!pFlac) {
      std::cerr << "Error: cannot open flac file: " << model_path << std::endl;
      return;
    }
    while (true) {
      DecompressFlacChunk(*pFlac, &pPCM, &frames);
      if (frames == 0)
        break;
      CompressChunk(model_path, pPCM, frames, pFlac->channels,
                    pFlac->sampleRate, buffers);
    }
    drflac_close(pFlac);
  } else {  /// .ogg
    std::cerr << "decompressing of .ogg is not possible by now" << std::endl;
    /*do {
      DecompressOggChunk(model_path, &pPCM, &frames, &channels, &sampleRate);
      if (frames == 0)
        break;
      PrepareEncodingContext(model_path, channels, sampleRate);
      CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
    } while (true);*/
  }
}

void AudioProcessor::ProcessSound(
    const std::filesystem::path& model_path,
    const std::filesystem::path& path_suffix __attribute__((unused))) {
  float* pPCM;
  uint64_t frames;

  drwav wav;
  if (!drwav_init_file(&wav, model_path.string().c_str(), nullptr)) {
    return;
  }
  while (true) {
    DecompressWavChunk(wav, &pPCM, &frames);
    if (frames == 0)
      break;
//    CompressChunk(model_path, pPCM, frames, wav.channels,
//                  wav.sampleRate, buffers);
  }
  drwav_uninit(&wav);
}

void AudioProcessor::DecompressMp3Chunk(drmp3& drmp3_context,
                                        float** pPCM, uint64_t* frames) {
  int channels = drmp3_context.channels;
  size_t buffer_size = faithful::config::audio_comp_buffer_size * 4;
  *pPCM = new float[buffer_size];
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drmp3_read_pcm_frames_f32(
              &drmp3_context, faithful::config::audio_comp_buffer_size,
              *pPCM + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + faithful::config::audio_comp_buffer_size) * channels){
      std::cerr << buffer_size << " "
                << ((*frames + faithful::config::audio_comp_buffer_size) * channels)
                << std::endl;
      break;
    }
  }
  std::cerr << "DecompressMp3Chunk" << std::endl;
}

void AudioProcessor::DecompressFlacChunk(drflac& drflac_context, float** pPCM,
                                         uint64_t* frames) {
  int channels = drflac_context.channels;
  size_t buffer_size = faithful::config::audio_comp_buffer_size * 8;
  *pPCM = new float[buffer_size];
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drflac_read_pcm_frames_f32(
              &drflac_context, faithful::config::audio_comp_buffer_size,
              *pPCM + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * faithful::config::audio_comp_buffer_size)
                          * channels)
      break;
  }
}

void AudioProcessor::DecompressWavChunk(drwav& drwav_context, float** pPCM,
                                        uint64_t* frames) {
  int channels = drwav_context.channels;
  size_t buffer_size = faithful::config::audio_comp_buffer_size * 8;
  *pPCM = new float[buffer_size];
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drwav_read_pcm_frames_f32(
              &drwav_context, faithful::config::audio_comp_buffer_size,
              *pPCM + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * faithful::config::audio_comp_buffer_size) * channels)
      break;
  }
}

void AudioProcessor::DecompressOggChunk(
    const std::filesystem::path& model_path __attribute__((unused)),
    float** pPCM __attribute__((unused)), uint64_t* frames __attribute__((unused)),
    int* channels __attribute__((unused)), int* sampleRate __attribute__((unused))) {
  // vorbisfile ?
}

void AudioProcessor::CompressChunk(
    const std::filesystem::path& model_path __attribute__((unused)),
    float* pPCM, uint64_t frames,
    int channels, int sampleRate __attribute__((unused)),
    std::vector<std::pair<long int, std::unique_ptr<char>>>& buffers) {
  std::cout
      << "_________" << '\n'
      << "frames:" << frames << '\n'
      << "channels:" << channels << '\n'
      << "sampleRate:" << sampleRate << '\n'
      << "_________" << std::endl;

  int thread_offset = frames / (worker_threads_count_ + 1);
  int qwerty = faithful::config::audio_comp_chunk_size;
  /////////////////
  while (!thread_pool_->thread_tasks_mutex_.try_lock()) {
  }
  std::vector<AudioProcessor::ThreadData> thread_data_(worker_threads_count_ + 1);

  std::cout << "LAst packetno: " << (*last_packetno_)[2] << std::endl;
  for (int i = 0; i < thread_data_.size(); ++i) {
    thread_data_[i].Init(channels, sampleRate);
//    thread_data_[i].op.granulepos = last_gramulepos_ + thread_offset * i;
//    thread_data_[i].op.packetno = (*last_packetno_)[i];
//    thread_data_[i].os.granulepos = last_gramulepos_ + thread_offset * i;
//    thread_data_[i].os.packetno = (*last_packetno_)[i];

    thread_data_[i].vd.granulepos = last_gramulepos_ + thread_offset * i;
    thread_data_[i].vd.sequence = (*last_packetno_)[i];
    thread_data_[i].vb.granulepos = last_gramulepos_ + thread_offset * i;
    thread_data_[i].vb.sequence = (*last_packetno_)[i];
  }
  // task to thread pool
  for (auto& b : buffers) {
    b.first = 0;
  }
  for (std::size_t k = 0; k < thread_data_.size() - 1; ++k) {
    thread_pool_->threads_tasks_[k].first =
        [&, k, qwerty, thread_offset]() {
          ThreadData& context = thread_data_[k];
          int next_pos = k * thread_offset * channels;
          int cur_pos;
          long int sentinel;

          float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);
          while (true) {
            cur_pos = next_pos;
            next_pos += qwerty * channels;
            sentinel = (k + 1) * thread_offset * channels - next_pos;
            if (sentinel >= 0) {
              sentinel = qwerty;
            } else {
              sentinel = qwerty + sentinel / channels;
            }

            for (int j = 0; j < channels; ++j) {
              for (long i = 0; i < sentinel; ++i) {
                buffer[j][i] = pPCM[cur_pos + i * channels + j];
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
                                buffers[k].second.get()  + buffers[k].first),
                            reinterpret_cast<void*>(context.og.header),
                            context.og.header_len);
                buffers[k].first += context.og.header_len;
                std::memcpy(reinterpret_cast<void*>(
                                buffers[k].second.get()  + buffers[k].first),
                            reinterpret_cast<void*>(context.og.body),
                            context.og.body_len);
                buffers[k].first += context.og.body_len;
              }
            }

            if (sentinel != qwerty) {
              std::cout << "sentinel != qwerty" << std::endl;
              break;
            }
          }
          std::cout << "TOTAL_" << k << "_: " << context.op.granulepos
                    << " " << context.op.packetno << std::endl;
        };
    thread_pool_->threads_tasks_[k].second = true;
  }

  thread_pool_->thread_tasks_mutex_.unlock();
  std::cout << "Now all threads are working" << std::endl;

  int context_idx = thread_data_.size() - 1;
  ThreadData& context = thread_data_[context_idx];
  int next_pos = worker_threads_count_ * thread_offset * channels;
  int cur_pos;
  long int sentinel;

  float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);
  while (true) {
    cur_pos = next_pos;
    next_pos += qwerty * channels;
    sentinel = frames * channels - next_pos;
//    std::cout << "SentineL: " << sentinel << std::endl;
    if (sentinel >= 0) {
      sentinel = qwerty;
    } else {
      sentinel = qwerty + sentinel / channels;
    }

    for (int j = 0; j < channels; ++j) {
      for (long i = 0; i < sentinel; ++i) {
        buffer[j][i] = pPCM[cur_pos + i * channels + j];
      }
    }
    vorbis_analysis_wrote(&context.vd, sentinel);
    while (vorbis_analysis_blockout(&context.vd, &context.vb) == 1) {
      vorbis_analysis(&context.vb, nullptr);
      vorbis_bitrate_addblock(&context.vb);  // Does we need it?
      vorbis_bitrate_flushpacket(&context.vd, &context.op);
      ogg_stream_packetin(&context.os, &context.op);
      while (ogg_stream_pageout(&context.os, &context.og)) {
        std::memcpy(reinterpret_cast<char*>(buffers[context_idx].second.get()) +
                                            buffers[context_idx].first,
                    reinterpret_cast<char*>(context.og.header),
                    context.og.header_len);
        buffers[context_idx].first += context.og.header_len;
//        std::cout << "first: " << context.op.granulepos
//                  << " " << context.op.packetno << std::endl;
        std::memcpy(reinterpret_cast<char*>(buffers[context_idx].second.get()) +
                                            buffers[context_idx].first,
                    reinterpret_cast<char*>(context.og.body),
                    context.og.body_len);
//        std::cout << "second: " << context.op.granulepos
//                  << " " << context.op.packetno << std::endl;
        buffers[context_idx].first += context.og.body_len;
      }
    }

    if (sentinel != qwerty) {
      std::cout << "sentinel != qwerty" << std::endl;
      break;
    }
  }
  std::cout << "TOTAL__: " << context.op.granulepos
            << " " << context.op.packetno << std::endl;

  while (!thread_pool_->Completed()) {
  }
  thread_pool_->UpdateContext();

  for (int i = 0; i < worker_threads_count_ + 1; ++i) {
    (*last_packetno_)[i] = thread_data_[i].op.packetno;
  }
  last_gramulepos_ += thread_offset * (worker_threads_count_ + 1);

  std::ofstream out_audio("audio_result.ogg", std::ios::binary | std::ios::app);
  for (const auto& b : buffers) {
    out_audio.write(b.second.get(), b.first);
  }
}

