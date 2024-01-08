#include "AudioProcessor.h"

#include <fstream>

//#define DR_MP3_IMPLEMENTATION
//#define DRMP3_IMPLEMENTATION
#include "dr_mp3.h"
#include "dr_flac.h"
#include "dr_wav.h"

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
    : encode_(encode),
      asset_destination_(asset_destination),
      thread_pool_(thread_pool),
      worker_threads_count_(thread_pool->get_thread_count()) {
}

AudioProcessor::~AudioProcessor() {  // TODO:
}

void AudioProcessor::ProcessMusic(const std::filesystem::path& model_path,
                                  const std::filesystem::path& path_suffix) {
  std::cout << "Music processing is not available by now" << std::endl;
  /*
   * TODO: segfault in AudioProcessor::CompressChunk()
   * TODO: flac, wav formats, ogg decompression -- easy
   * TODO: smart allocations (all: ogg, vorbis, dr_libs)
   *
   * */
  return;
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

void AudioProcessor::EncodeMusic(const std::filesystem::path& model_path,
                                 const std::filesystem::path& path_suffix) {
  float* pPCM;
  uint64_t frames;
  int channels;
  int sampleRate;
  /// loop for really huge files (in most cases only 1 iteration)

  int total_thread = worker_threads_count_ + 1;
  int buffer_size = frames_decompress_count_ * channels *
                    100;  // average packet size TODO: too hard...
  std::vector<std::pair<long int, char*>> buffers(total_thread);
  for (auto& b : buffers)
    b = std::make_pair(0, new char[buffer_size]);  // TODO: unique ptr

  if (model_path.extension() == ".mp3") {
    drmp3 drmp3_context;
    if (!drmp3_init_file(&drmp3_context, model_path.c_str(),
                         nullptr)) {  // TODO: allocation
      std::cout << "Error: cannot open mp3 file: " << model_path << std::endl;
      return;
    }
    channels = drmp3_context.channels;
    sampleRate = drmp3_context.sampleRate;
    PrepareEncodingContext(model_path, channels, sampleRate);
    while (true) {
      DecompressMp3Chunk(drmp3_context, channels, &pPCM, &frames);
      if (frames == 0)
        break;
      CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
      std::ofstream out_audio("audio_result.ogg",
                              std::ios::binary | std::ios::app);
      //      for (auto& b : buffers)
      //        out_audio.write(b.second, b.first);
      out_audio.write(buffers[7].second, buffers[7].first);
      std::cout << "FFFFFFFFFFFFFFFF" << buffers[7].first << std::endl;
      // TODO: write data to file
    }
    drmp3_uninit(&drmp3_context);
    // TODO: drmp3_free(pPCM, nullptr);
  } else if (model_path.extension() == ".flac") {
    do {
      DecompressFlacChunk(model_path, &pPCM, &frames, &channels, &sampleRate);
      if (frames == 0)
        break;
      PrepareEncodingContext(model_path, channels, sampleRate);
      CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
    } while (true);
  } else {  /// .ogg
    do {
      DecompressOggChunk(model_path, &pPCM, &frames, &channels, &sampleRate);
      if (frames == 0)
        break;
      PrepareEncodingContext(model_path, channels, sampleRate);
      CompressChunk(model_path, pPCM, frames, channels, sampleRate, buffers);
    } while (true);
  }
}
void AudioProcessor::DecodeMusic(const std::filesystem::path& model_path,
                                 const std::filesystem::path& path_suffix) {
  std::cout << "Decoded music" << std::endl;
}

void AudioProcessor::EncodeSound(const std::filesystem::path& model_path,
                                 const std::filesystem::path& path_suffix) {
  // TODO: here only .wav format
  std::cout << "Encoded sound" << std::endl;
}
void AudioProcessor::DecodeSound(const std::filesystem::path& model_path,
                                 const std::filesystem::path& path_suffix) {
  // TODO: here only .wav format
  std::cout << "Decoded sound" << std::endl;
}

void AudioProcessor::DecompressMp3Chunk(drmp3& drmp3_context, int channels,
                                        float** pPCM, uint64_t* frames) {
  size_t buffer_size = frames_decompress_count_ * channels *
                       800;  // / 100; // TODO: ............
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
  std::cout << buffer_size << "<--here" << std::endl;  // 50 000 000
  std::cout << "before disaster" << std::endl;
}

void AudioProcessor::DecompressFlacChunk(
    const std::filesystem::path& model_path, float** pPCM, uint64_t* frames,
    int* channels, int* sampleRate) {
  drflac* pFlac = drflac_open_file(model_path.c_str(), nullptr);
  if (pFlac == nullptr) {
    return;
  }

  *channels = pFlac->channels;
  *sampleRate = pFlac->sampleRate;

  size_t initialBufferSize = frames_decompress_count_ * (*channels);
  *pPCM = (float*)malloc(sizeof(float) * initialBufferSize);

  *frames = 0;
  size_t framesRead;
  while ((framesRead = drflac_read_pcm_frames_f32(
              pFlac, frames_decompress_count_,
              *pPCM + (*frames * (*channels)))) > 0) {
    *frames += framesRead;
    size_t newSize = (*frames + frames_decompress_count_) * (*channels);
    if (newSize > initialBufferSize) {
      *pPCM = (float*)realloc(*pPCM, sizeof(float) * newSize);
      initialBufferSize = newSize;
    }
  }
  drflac_close(pFlac);
}

void AudioProcessor::DecompressWavChunk(const std::filesystem::path& model_path,
                                        float** pPCM, uint64_t* frames,
                                        int* channels, int* sampleRate) {
  drwav wav;
  if (!drwav_init_file(&wav, model_path.c_str(), nullptr)) {
    return;
  }

  *channels = wav.channels;
  *sampleRate = wav.sampleRate;

  size_t initialBufferSize = frames_decompress_count_ * (*channels);
  *pPCM = (float*)malloc(sizeof(float) * initialBufferSize);

  *frames = 0;
  size_t framesRead;
  while ((framesRead =
              drwav_read_pcm_frames_f32(&wav, frames_decompress_count_,
                                        *pPCM + (*frames * (*channels)))) > 0) {
    *frames += framesRead;
    size_t newSize = (*frames + frames_decompress_count_) * (*channels);
    if (newSize > initialBufferSize) {
      *pPCM = (float*)realloc(*pPCM, sizeof(float) * newSize);
      initialBufferSize = newSize;
    }
  }
  drwav_uninit(&wav);
}

void AudioProcessor::DecompressOggChunk(const std::filesystem::path& model_path,
                                        float** pPCM, uint64_t* frames,
                                        int* channels, int* sampleRate) {
  // vorbisfile ?
}

void AudioProcessor::CompressChunk(
    const std::filesystem::path& model_path, float* pPCM, uint64_t frames,
    int channels, int sampleRate,
    std::vector<std::pair<long int, char*>>& buffers) {
  std::cout << "disaster" << std::endl;

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

  for (int k = 0; k < thread_data_->size() - 1; ++k) {
    thread_pool_->threads_tasks_[k].first =
        std::move([&, k, qwerty, thread_offset]() {
          return;
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
              for (uint64_t i = 0; i < sentinel; ++i) {
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
                                buffers[k].second /* + buffers[k].first*/),
                            reinterpret_cast<void*>(context.og.header),
                            0);  // context.og.header_len);
                buffers[k].first += context.og.header_len;
                std::memcpy(reinterpret_cast<void*>(
                                buffers[k].second /* + buffers[k].first*/),
                            reinterpret_cast<void*>(context.og.body),
                            0);  // context.og.body_len);
                buffers[k].first += context.og.body_len;
              }
            }

            if (sentinel != qwerty) {
              std::cout << "sentinel != qwerty" << std::endl;
              break;
            }
          }
        });
    thread_pool_->threads_tasks_[k].second = true;
  }
  thread_pool_->thread_tasks_mutex_.unlock();

  int context_idx = thread_data_->size() - 1;
  ThreadData& context = (*thread_data_)[context_idx];
  int next_pos = worker_threads_count_ * thread_offset;
  int cur_pos;
  long int sentinel;

  float** buffer = vorbis_analysis_buffer(&context.vd, qwerty);
  while (true) {
    cur_pos = next_pos;
    next_pos += qwerty * channels;  // TODO: instead of checking every time
                                    // precalculate all pair<start, len>
    sentinel = frames * channels - next_pos;
    if (sentinel >= 0) {
      sentinel = qwerty;
    } else {
      sentinel = qwerty + sentinel / channels;
    }

    for (int j = 0; j < channels; ++j) {
      for (uint64_t i = 0; i < sentinel; ++i) {
        buffer[j][i] = pPCM[cur_pos + i * channels + j];
      }
    }

    //    std::ofstream out_audio("audio_result.ogg", std::ios::out |
    //    std::ios::app);

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
        //        out_audio.write(reinterpret_cast<const
        //        char*>(context.og.header), context.og.header_len);
        //        out_audio.write(reinterpret_cast<const
        //        char*>(context.og.body), context.og.body_len);
        std::memcpy(reinterpret_cast<void*>(buffers[context_idx].second +
                                            buffers[context_idx].first),
                    reinterpret_cast<void*>(context.og.header),
                    context.og.header_len / 8);
        buffers[context_idx].first += context.og.header_len / 8;
        std::memcpy(reinterpret_cast<void*>(buffers[context_idx].second +
                                            buffers[context_idx].first),
                    reinterpret_cast<void*>(context.og.body),
                    context.og.body_len / 8);
        buffers[context_idx].first += context.og.body_len / 8;
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
  //  std::ofstream out_audio("audio_result.ogg", std::ios::out |
  //  std::ios::app); for (auto& i : buffers) {
  //    out_audio.write(i, buffer_size);
  //  }
  // TODO: add to file/main_buffer
  // TODO: delete[];
}

void AudioProcessor::PrepareEncodingContext(
    const std::filesystem::path& model_path, int channels, int sampleRate) {
  if (!initialized_) {
    thread_data_ = new std::vector<ThreadData>(worker_threads_count_ + 1);
  }

  bool data_written = false;
  int t = 0;
  for (auto& data : *thread_data_) {
    data.Init(channels, sampleRate);

    if (!data_written) {
      std::ofstream out_file("audio_result.ogg", std::ios::binary);
      while (ogg_stream_flush(&data.os, &data.og)) {
        out_file.write(reinterpret_cast<const char*>(data.og.header),
                       data.og.header_len);
        out_file.write(reinterpret_cast<const char*>(data.og.body),
                       data.og.body_len);
      }
      data_written = true;
    } else {
      while (ogg_stream_flush(&data.os, &data.og)) {
      }
    }
    ++t;
    // TODO:
    //      vorbis_comment_clear(&vc_);
    //      vorbis_info_clear(&vi_);
  }
  initialized_ = true;
}
