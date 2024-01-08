#ifndef ASSETPROCESSOR_AUDIOPROCESSOR_H
#define ASSETPROCESSOR_AUDIOPROCESSOR_H

#include <filesystem>
#include <vector>

#include <iostream>

#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/codec.h"

#include "dr_mp3.h"

#include "../../config/AssetFormats.h"

class AssetLoadingThreadPool;

// TODO: clang-tidy, clang-format

class AudioProcessor {
 public:
  struct ThreadData {
    void Init(int channels, int sampleRate) {
      if (initialized)
        return;
      vorbis_info_init(&vi);
      vorbis_comment_init(&vc);
      vorbis_comment_add_tag(&vc, "PROJECT", "Faithful");
      vorbis_encode_init_vbr(&vi, channels, sampleRate,
                             faithful::config::audio_comp_quality);

      ogg_stream_init(&os, 0);  // TODO: replace rand (or not...)
      vorbis_analysis_init(&vd, &vi);
      vorbis_block_init(&vd, &vb);
      vorbis_analysis_headerout(&vd, &vc, &ogg_header, &ogg_header_comm,
                                &ogg_header_code);
      ogg_stream_packetin(&os, &ogg_header);
      ogg_stream_packetin(&os, &ogg_header_comm);
      ogg_stream_packetin(&os, &ogg_header_code);
      initialized = true;
    }
    ogg_stream_state os;
    vorbis_dsp_state vd;
    vorbis_block vb;
    ogg_packet op;
    ogg_page og;
    vorbis_info vi;
    vorbis_comment vc;

    ogg_packet ogg_header;
    ogg_packet ogg_header_comm;
    ogg_packet ogg_header_code;

   private:
    bool initialized = false;
  };

  AudioProcessor(bool encode, const std::filesystem::path& asset_destination,
                 AssetLoadingThreadPool* thread_pool);

  ~AudioProcessor();

  void ProcessMusic(const std::filesystem::path& model_path,
                    const std::filesystem::path& path_suffix);
  void ProcessSound(const std::filesystem::path& model_path,
                    const std::filesystem::path& path_suffix);

 private:
  void EncodeMusic(const std::filesystem::path& model_path,
                   const std::filesystem::path& path_suffix);
  void DecodeMusic(const std::filesystem::path& model_path,
                   const std::filesystem::path& path_suffix);

  void EncodeSound(const std::filesystem::path& model_path,
                   const std::filesystem::path& path_suffix);
  void DecodeSound(const std::filesystem::path& model_path,
                   const std::filesystem::path& path_suffix);

  void DecompressMp3Chunk(drmp3& drmp3_context, int channels, float** pPCM,
                          uint64_t* frames);
  void DecompressFlacChunk(const std::filesystem::path& model_path,
                           float** pPCM, uint64_t* frames, int* channels,
                           int* sampleRate);
  void DecompressWavChunk(const std::filesystem::path& model_path, float** pPCM,
                          uint64_t* frames, int* channels, int* sampleRate);
  void DecompressOggChunk(const std::filesystem::path& model_path, float** pPCM,
                          uint64_t* frames, int* channels, int* sampleRate);

  void CompressChunk(const std::filesystem::path& model_path, float* pPCM,
                     uint64_t frames, int channels, int sampleRate,
                     std::vector<std::pair<long int, char*>>& buffers);

  void PrepareEncodingContext(const std::filesystem::path& model_path,
                              int channels, int sampleRate);

  AssetLoadingThreadPool* thread_pool_ = nullptr;

  /// storing there for convenience
  /// (we could ask thread_pool_ for it each time, but this is better)
  int worker_threads_count_;
  std::filesystem::path asset_destination_;

  std::vector<ThreadData>* thread_data_;

  int chunk_compress_size_ = faithful::config::audio_comp_chunk_size;
  int frames_decompress_count_ = faithful::config::audio_decomp_frames_count;

  bool initialized_ = false;
  bool encode_;
};

#endif  // ASSETPROCESSOR_AUDIOPROCESSOR_H
