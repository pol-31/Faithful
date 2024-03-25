#include "AudioProcessor.h"

#include <iostream>

#include "AssetLoadingThreadPool.h"

#include "../../config/AssetFormats.h"

/* Audio Processing is NOT SUPPORTED by now:
 * Issue#1
 *  we create n=thread_num files, each has its own header which is redundant
 *  while need only one
 * Issue#2
 *  artifacts between threads (MDCT for equal-sized windows)
 *  TODO: for some reason I can't handle this nice and we still have
 *   such artifacts
 * */

void AudioProcessor::ThreadData::Init(int channels, int sample_rate, int id) {
  vorbis_info_init(&vi);
  vorbis_comment_init(&vc);
  vorbis_encode_init_vbr(&vi, channels, sample_rate,
                         faithful::config::kAudioCompQuality);

  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);
  // don't need srand(time) because we simply concatenating audio
  ogg_stream_init(&os, 0);

  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;

  vorbis_analysis_headerout(&vd, &vc, &header,
                            &header_comm, &header_code);
  ogg_stream_packetin(&os, &header);
  ogg_stream_packetin(&os, &header_comm);
  ogg_stream_packetin(&os, &header_code);

  std::filesystem::path filename{std::filesystem::current_path() / "temp"};
  std::string summary_file{"audio_result"};
  std::string extension{".ogg"};

  std::ofstream out_file(filename / (summary_file + std::to_string(id) + extension), std::ios::binary);
  while (ogg_stream_flush(&os, &og)) {
    out_file.write(reinterpret_cast<const char*>(og.header),
                   og.header_len);
    out_file.write(reinterpret_cast<const char*>(og.body),
                   og.body_len);
  }
}

AudioProcessor::ThreadData::~ThreadData() {
  ogg_stream_clear(&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  vorbis_comment_clear(&vc);
  vorbis_info_clear(&vi);
}

AudioProcessor::AudioProcessor(
    AssetLoadingThreadPool& thread_pool,
    ReplaceRequest& replace_request)
    : thread_pool_(thread_pool),
      thread_number_(thread_pool_.GetThreadNumber()),
      replace_request_(replace_request) {
//  thread_buffers_.resize(thread_number_);
  std::filesystem::create_directories(std::filesystem::current_path() / "temp");
  // thread_info_ resized for each new file
}

void AudioProcessor::SetDestinationDirectory(
    const std::filesystem::path& path) {
  sounds_destination_path_ = path / "sounds";
  music_destination_path_ = path / "music";
  std::filesystem::create_directories(sounds_destination_path_);
  std::filesystem::create_directories(music_destination_path_);
}

void AudioProcessor::Encode(const std::filesystem::path& path) {
  std::cerr << "AudioProcessor::Process is no implemented" << std::endl;
  std::terminate();
  AudioSize audio_size = AudioSize::kMusic;
  AudioFormat audio_format;
  if (path.extension() == ".flac") {
    audio_format = AudioFormat::kFlac;
    if (file_size(path) < faithful::config::kMusicFlacThreshold) {
      audio_size = AudioSize::kSound;
    }
  } else if (path.extension() == ".ogg") {
    audio_format = AudioFormat::kOgg;
    if (file_size(path) < faithful::config::kMusicOggThreshold) {
      audio_size = AudioSize::kSound;
    } else {
      // TODO: just copy
      return;
    }
  } else if (path.extension() == ".mp3") {
    audio_format = AudioFormat::kMp3;
    if (file_size(path) < faithful::config::kMusicMp3Threshold) {
      audio_size = AudioSize::kSound;
    }
  } else if (path.extension() == ".wav") {
    audio_format = AudioFormat::kWav;
    if (file_size(path) < faithful::config::kMusicWavThreshold) {
      // TODO: just copy
      return;
    }
  } else {
    throw;
  }
  // TODO: there we should have: format, music/sound, out_filename
  //  there we ReplaceRequest()
  //  there we Encode()
}

void AudioProcessor::Decode(const std::filesystem::path& path) {
  /// audio already in listenable format, so just copy
  std::filesystem::path out_filename;
  if (path.extension() == ".ogg") {
    out_filename = sounds_destination_path_ / path.filename();
  } else if (path.extension() == ".wav") {
    out_filename = music_destination_path_ / path.filename();
  } else {
    throw;
  }
  if (std::filesystem::exists(out_filename)) {
    std::string request{out_filename};
    request += "\nalready exist. Do you want to replace it?";
    if (!replace_request_(std::move(request))) {
      return;
    }
  }
  std::filesystem::rename(path, out_filename);
}

// TODO: replace request before processing, not before writing

void AudioProcessor::EncodeMusic(const std::filesystem::path& audio_path) {
  std::cout << "Music processing: " << audio_path << std::endl;
  size_t buffer_size = faithful::config::kAudioTotalChunkBufferSize;
  auto pcm_data = std::make_unique<float[]>(buffer_size);
  uint64_t frames;

  cur_file_.open("result.ogg", std::ios::binary);


/*
  InitThreadBuffers();
  PrepareCompressedOggDataFile("");*/

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

      std::cerr << "--- between --- " << std::endl;
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
      if (frames == 0) {
        break;
      }
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
    for (std::size_t i = 0; i < thread_info_.size(); ++i) {
      thread_info_[i].Init(channels, sample_rate, i);
    }
  }
}

void AudioProcessor::EncodeSound(const std::filesystem::path& audio_path) {
  // TODO: don't need buffer for Sound
  size_t buffer_size = faithful::config::kAudioTotalChunkBufferSize;
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
              &drwav_context, faithful::config::kAudioDecompChunkSize,
              pcm_data.get() + (frames * channels))) > 0) {
    frames += framesRead;
    if (buffer_size < (frames + 2 * faithful::config::kAudioDecompChunkSize) * channels) {
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
              &drmp3_context, faithful::config::kAudioDecompChunkSize,
              pcm_data + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + faithful::config::kAudioDecompChunkSize) * channels){
      std::cerr
          << buffer_size << " "
          << *frames << " "
          << faithful::config::kAudioDecompChunkSize << " "
          << channels << " "
          << ((*frames + faithful::config::kAudioDecompChunkSize) * channels)
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
              &drflac_context, faithful::config::kAudioDecompChunkSize,
              pcm_data + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * faithful::config::kAudioDecompChunkSize)
                          * channels)
      break;
  }
}

void AudioProcessor::CompressChunk(
    float* pcm_data, uint64_t frames,
    int channels, int sample_rate) {
  int thread_offset = frames / thread_number_;

  int qwerty = faithful::config::kAudioCompChunkSize;

  std::filesystem::path filename{std::filesystem::current_path() / "temp"};
  std::string summary_file{"audio_result"};
  std::string extension{".ogg"};

  folly::Function<void(int)> task = {
      [&, qwerty, thread_offset](int thread_id) {
        std::filesystem::path local_filename = filename / (summary_file + std::to_string(thread_id) + extension);
        std::ofstream file(local_filename, std::ios::app);
        // TODO: if !open() ...
        ThreadData& context = thread_info_[thread_id];
        int cur_pos, next_pos;
        long int sentinel;

        /// if current piece is small enough to process with single thread
        if (thread_id == -1) {
          //
          std::terminate();
        }

        next_pos = thread_id * thread_offset * channels;
        float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);

        /// overlapped 1 - should be applied for all thread_id != 0
        /*cur_pos = next_pos;
        next_pos += qwerty * channels;
        for (int j = 0; j < channels; ++j) {
          for (long i = 0; i < faithful::config::kAudioCompChunkSize; ++i) {
            buffer[j][i] = pcm_data[cur_pos + i * channels + j];
          }
        }
        vorbis_analysis_wrote(&context.vd, faithful::config::kAudioCompChunkSize);

        while (vorbis_analysis_blockout(&context.vd, &context.vb) == 1) {
          vorbis_analysis(&context.vb, nullptr);
          vorbis_bitrate_addblock(&context.vb);
          vorbis_bitrate_flushpacket(&context.vd, &context.op);
          ogg_stream_packetin(&context.os, &context.op);
        }
        ogg_stream_flush(&context.os, &context.og);
        file.write(reinterpret_cast<char*>(context.og.body + (context.og.body_len/4) * 3),
                   context.og.body_len/4);*/

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
//            next_pos = (thread_id + 1) * thread_offset * channels;
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
            vorbis_bitrate_addblock(&context.vb);
            vorbis_bitrate_flushpacket(&context.vd, &context.op);
            ogg_stream_packetin(&context.os, &context.op);
            while (ogg_stream_pageout(&context.os, &context.og)) {
              file.write(reinterpret_cast<char*>(context.og.header),
                         context.og.header_len);
              file.write(reinterpret_cast<char*>(context.og.body),
                         context.og.body_len);
            }
          }
          if (sentinel != qwerty) {
            break;
          }
        }

        /// last flush?
/*      ogg_stream_flush(&context.os, &context.og);
        ogg_stream_pageout(&context.os, &context.og);
        file.write(reinterpret_cast<char*>(context.og.header),
                   context.og.header_len);
        file.write(reinterpret_cast<char*>(context.og.body),
                   context.og.body_len);*/

        /// overlapped 2 - should be applied for all thread_id != thread_num-1
        /*if (thread_id != (thread_number_ - 1)) {
          for (int j = 0; j < channels; ++j) {
            for (long i = 0; i < faithful::config::kAudioCompChunkSize; ++i) {
              buffer[j][i] = pcm_data[next_pos + i * channels + j];
            }
          }
          vorbis_analysis_wrote(&context.vd, faithful::config::kAudioCompChunkSize);

          while (vorbis_analysis_blockout(&context.vd, &context.vb) == 1) {
            vorbis_analysis(&context.vb, nullptr);
            vorbis_bitrate_addblock(&context.vb);
            vorbis_bitrate_flushpacket(&context.vd, &context.op);
            ogg_stream_packetin(&context.os, &context.op);
          }
          ogg_stream_flush(&context.os, &context.og);
          file.write(reinterpret_cast<char*>(context.og.header),
                     context.og.header_len);
          file.write(reinterpret_cast<char*>(context.og.body),
                     (context.og.body_len/4) * 3);
        }*/
      }
  };

  if (frames < faithful::config::kAudioCompThreshold) {
    task(-1);
  } else {
    std::cout << "--- right ---" << std::endl;
    thread_pool_.Execute(std::move(task));
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "set done" << std::endl;

  WriteCompressedOggChunks(thread_offset);
}

void AudioProcessor::WriteCompressedOggChunks(int thread_offset) {
  std::ifstream inputFile1("temp/audio_result0.ogg", std::ios::binary);
  if (!inputFile1) {
    std::cerr << "Failed to open input file 1 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile1.rdbuf();
  inputFile1.close();

  std::ifstream inputFile2("temp/audio_result1.ogg", std::ios::binary);
  if (!inputFile2) {
    std::cerr << "Failed to open input file 2 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile2.rdbuf();
  inputFile2.close();

  std::ifstream inputFile3("temp/audio_result2.ogg", std::ios::binary);
  if (!inputFile3) {
    std::cerr << "Failed to open input file 3 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile3.rdbuf();
  inputFile3.close();

  std::ifstream inputFile4("temp/audio_result3.ogg", std::ios::binary);
  if (!inputFile4) {
    std::cerr << "Failed to open input file 4 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile4.rdbuf();
  inputFile4.close();

  std::ifstream inputFile5("temp/audio_result4.ogg", std::ios::binary);
  if (!inputFile5) {
    std::cerr << "Failed to open input file 5 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile5.rdbuf();
  inputFile5.close();

  std::ifstream inputFile6("temp/audio_result5.ogg", std::ios::binary);
  if (!inputFile6) {
    std::cerr << "Failed to open input file 6 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile6.rdbuf();
  inputFile6.close();

  std::ifstream inputFile7("temp/audio_result6.ogg", std::ios::binary);
  if (!inputFile7) {
    std::cerr << "Failed to open input file 7 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile7.rdbuf();
  inputFile7.close();

  std::ifstream inputFile8("temp/audio_result7.ogg", std::ios::binary);
  if (!inputFile8) {
    std::cerr << "Failed to open input file 8 for reading" << std::endl;
    return;
  }
  cur_file_ << inputFile8.rdbuf();
  inputFile8.close();

  return;
}
