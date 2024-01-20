#include "AudioProcessor.h"

#include <fstream>
#include <memory>

// TODO: check dr_lib/vorbis macros

// TODO:
//  #define _ogg_malloc  malloc
//  #define _ogg_calloc  calloc
//  #define _ogg_realloc realloc
//  #define _ogg_free    free

#include "AssetLoadingThreadPool.h"

#include "../../config/AssetFormats.h"

AudioProcessor::AudioProcessor(bool encode,
                               const std::filesystem::path& asset_destination,
                               AssetLoadingThreadPool* thread_pool)
    : asset_destination_(asset_destination),
      thread_pool_(thread_pool),
      worker_threads_count_(thread_pool->get_thread_count()),
      encode_(encode) {
}

AudioProcessor::~AudioProcessor() {  // TODO:
}

void AudioProcessor::ProcessMusic(const std::filesystem::path& model_path,
                                  const std::filesystem::path& path_suffix) {
  std::cout << "Music processing ..." << std::endl;
//  std::cout << "Music processing is not available by now" << std::endl;
  /*
   * TODO: segfault in AudioProcessor::CompressChunk()
   * TODO: flac, wav formats, ogg decompression -- easy
   * TODO: smart allocations (all: ogg, vorbis, dr_libs)
   *
   * */
  if (encode_) {
    EncodeMusic(model_path, path_suffix);
  } else {
    DecodeMusic(model_path, path_suffix);
  }
}

void AudioProcessor::ProcessSound(const std::filesystem::path& model_path,
                                  const std::filesystem::path& path_suffix) {
  std::cout << "Sound processing is not available by now" << std::endl;
  return;
  if (encode_) {
    EncodeSound(model_path, path_suffix);
  } else {
    DecodeSound(model_path, path_suffix);
  }
}

void AudioProcessor::EncodeMusic(
    const std::filesystem::path& model_path,
    const std::filesystem::path& path_suffix __attribute__((unused))) {
  float* pPCM;
  uint64_t frames;
  int channels = 2;
  int sampleRate;
  /// loop for really huge files (in most cases only 1 iteration)

  int total_thread = worker_threads_count_ + 1;
  int buffer_size = frames_decompress_count_ * channels *
                    1000;  // average packet size TODO: too hard...
  std::cout << "frames_decompress_count_ " << frames_decompress_count_ << std::endl;
  std::cout << "channels " << channels << std::endl;
  std::cout << "BUFFER SIZE " << buffer_size << std::endl;
  std::vector<std::pair<long int, std::unique_ptr<char>>> buffers(total_thread);
  for (auto& b : buffers)
    b = std::make_pair(0, std::unique_ptr<char>(new char[buffer_size]));

  if (model_path.extension() == ".mp3") {
    drmp3 drmp3_context;
    if (!drmp3_init_file(&drmp3_context, model_path.string().c_str(),
                         nullptr)) {  // TODO: allocation
      std::cout << "Error: cannot open mp3 file: " << model_path << std::endl;
      return;
    }
    channels = drmp3_context.channels;
    sampleRate = drmp3_context.sampleRate;
    PrepareEncodingContext(model_path, channels, sampleRate);
    while (true) {
      DecompressMp3Chunk(drmp3_context, &pPCM, &frames);
      if (frames == 0)
        break;
      CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
    }
    drmp3_uninit(&drmp3_context);
    // TODO: drmp3_free(pPCM, nullptr);
  } else if (model_path.extension() == ".flac") {
    drflac* pFlac = drflac_open_file(model_path.string().c_str(), nullptr);
    if (pFlac == nullptr) {
      return;
    }
    channels = pFlac->channels;
    sampleRate = pFlac->sampleRate;
    PrepareEncodingContext(model_path, channels, sampleRate);
    while (true) {
      DecompressFlacChunk(*pFlac, &pPCM, &frames);
      if (frames == 0)
        break;
      CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
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
void AudioProcessor::DecodeMusic(
    const std::filesystem::path& model_path __attribute__((unused)),
    const std::filesystem::path& path_suffix __attribute__((unused))) {
  std::cout << "Decoded music" << std::endl;
}

void AudioProcessor::EncodeSound(
    const std::filesystem::path& model_path __attribute__((unused)),
    const std::filesystem::path& path_suffix __attribute__((unused))) {
  std::cout << "Sound encoding ..." << std::endl;
  float* pPCM;
  uint64_t frames;
  int channels = 2;
  int sampleRate;

  int buffer_size = frames_decompress_count_ * channels *
                    1000;  // average packet size TODO: too hard...
  std::cout << "frames_decompress_count_ " << frames_decompress_count_ << std::endl;
  std::cout << "channels " << channels << std::endl;
  std::cout << "BUFFER SIZE " << buffer_size << std::endl;
  auto buffer = std::make_pair(0, std::unique_ptr<char>(new char[buffer_size]));

  drwav wav;
  if (!drwav_init_file(&wav, model_path.string().c_str(), nullptr)) {
    return;
  }
  channels = wav.channels;
  sampleRate = wav.sampleRate;
  PrepareEncodingContext(model_path, channels, sampleRate);
  while (true) {
    DecompressWavChunk(wav, &pPCM, &frames);
    if (frames == 0)
      break;
    // TODO: CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
  }
  drwav_uninit(&wav);
}
void AudioProcessor::DecodeSound(
    const std::filesystem::path& model_path __attribute__((unused)),
    const std::filesystem::path& path_suffix __attribute__((unused))) {
  // TODO: here only .wav format
  std::cout << "Decoded sound" << std::endl;
}

void AudioProcessor::DecompressMp3Chunk(drmp3& drmp3_context,
                                        float** pPCM, uint64_t* frames) {
  int channels = drmp3_context.channels;
  size_t buffer_size = frames_decompress_count_ * channels *
                       100000;  // / 100; // TODO: ............
  *pPCM = new float[buffer_size];
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drmp3_read_pcm_frames_f32(
              &drmp3_context, frames_decompress_count_,
              *pPCM + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * frames_decompress_count_) * channels)
      break;
  }
}

void AudioProcessor::DecompressFlacChunk(drflac& drflac_context, float** pPCM,
                                         uint64_t* frames) {
  int channels = drflac_context.channels;
  size_t buffer_size = frames_decompress_count_ * channels *
                       100000;  // / 100; // TODO: ............
  *pPCM = new float[buffer_size];
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drflac_read_pcm_frames_f32(
              &drflac_context, frames_decompress_count_,
              *pPCM + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * frames_decompress_count_) * channels)
      break;
  }
}

void AudioProcessor::DecompressWavChunk(drwav& drwav_context, float** pPCM,
                                        uint64_t* frames) {
  int channels = drwav_context.channels;
  size_t buffer_size = frames_decompress_count_ * channels *
                       100000;  // / 100; // TODO: ............
  *pPCM = new float[buffer_size];
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drwav_read_pcm_frames_f32(
              &drwav_context, frames_decompress_count_,
              *pPCM + (*frames * channels))) > 0) {
    *frames += framesRead;
    if (buffer_size < (*frames + 2 * frames_decompress_count_) * channels)
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
  int qwerty = frames_decompress_count_;  // TODO: AssetFormats.h
  /////////////////
  while (!thread_pool_->thread_tasks_mutex_.try_lock()) {
  }
  // task to thread pool
  for (auto& b : buffers) {
    b.first = 0;
    std::cout << "clearing of context" << std::endl;
  }
  for (std::size_t k = 0; k < thread_data_->size() - 1; ++k) {
//    continue;
    thread_pool_->threads_tasks_[k].first =
        [&, k, qwerty, thread_offset]() {
          ThreadData& context = (*thread_data_)[k];
          int next_pos = k * thread_offset * channels;
          int cur_pos;
          long int sentinel;

          float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);
          while (true) {
            cur_pos = next_pos;
            next_pos +=
                qwerty * channels;  // TODO: instead of checking every time
                                    // precalculate all pair<start, len>
            sentinel = (k + 1) * thread_offset * channels - next_pos;
            if (sentinel >= 0) {
              sentinel = qwerty;
            } else {
              sentinel = qwerty + sentinel / channels;
            }

            for (int j = 0; j < channels; ++j) {
              for (long i = 0; i < sentinel; ++i) {
                buffer[j][i] =
                    pPCM[cur_pos + i * channels +
                         j];  // TODO: thread distinct
                              // offset--------------------------------------
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
        };
    thread_pool_->threads_tasks_[k].second = true;
  }

  thread_pool_->thread_tasks_mutex_.unlock();
  std::cout << "Now all threads are working" << std::endl;

  int context_idx = thread_data_->size() - 1;
  ThreadData& context = (*thread_data_)[context_idx];
  int next_pos = worker_threads_count_ * thread_offset * channels;
  int cur_pos;
  long int sentinel;

  float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);
  while (true) {
    cur_pos = next_pos;
    next_pos += qwerty * channels;
    sentinel = frames * channels - next_pos;
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
        if (buffers[context_idx].first < 0)
          break;
        std::cout << "buffer " << buffers[context_idx].first << std::endl;

        std::memcpy(reinterpret_cast<char*>(buffers[context_idx].second.get()) +
                                            buffers[context_idx].first,
                    reinterpret_cast<char*>(context.og.header),
                    context.og.header_len);
        buffers[context_idx].first += context.og.header_len;
        std::memcpy(reinterpret_cast<char*>(buffers[context_idx].second.get()) +
                                            buffers[context_idx].first,
                    reinterpret_cast<char*>(context.og.body),
                    context.og.body_len);
        buffers[context_idx].first += context.og.body_len;
      }
    }

    if (sentinel != qwerty) {
      std::cout << "sentinel != qwerty" << std::endl;
      break;
    }
  }
  //  ogg_stream_clear(&context.os);
  //  vorbis_block_clear(&context.vb);
  //  vorbis_dsp_clear(&context.vd);

  while (!thread_pool_->Completed()) {
  }
  thread_pool_->UpdateContext();

  std::ofstream out_audio("audio_result.ogg", std::ios::binary | std::ios::app);
//  context_idx = 7;
//  out_audio.write(buffers[context_idx].second.get(), buffers[context_idx].first);
  for (const auto& b : buffers) {
    out_audio.write(b.second.get(), b.first);
  }
  // TODO: add to file/main_buffer
  // TODO: delete[];
}

void AudioProcessor::PrepareEncodingContext(
    const std::filesystem::path& model_path __attribute__((unused)),
    int channels, int sampleRate) {
  if (!initialized_) {
    std::cout << "initializing in PrepareEncodingContext" << std::endl;
    thread_data_ = new std::vector<ThreadData>(worker_threads_count_ + 1);
  }
  std::cout << "SKIP initializing in PrepareEncodingContext" << std::endl;

  bool data_written = false;
  for (auto& data : *thread_data_) {
    data.Init(channels, sampleRate);
    std::ofstream out_file("audio_result.ogg", std::ios::binary);
    if (!data_written) {
      while (ogg_stream_flush(&data.os, &data.og)) {
        out_file.write(reinterpret_cast<const char*>(data.og.header),
                       data.og.header_len);
        out_file.write(reinterpret_cast<const char*>(data.og.body),
                       data.og.body_len);
      }
//      data_written = true;
//    } else {
      //while (ogg_stream_flush(&data.os, &data.og)) {
      //}
    }
    continue;



    // TODO:
    //      vorbis_comment_clear(&vc_);
    //      vorbis_info_clear(&vi_);
  }
  initialized_ = true;
}
