#ifndef ASSETPROCESSOR_AUDIOPROCESSOR_H
#define ASSETPROCESSOR_AUDIOPROCESSOR_H

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

#include <dr_mp3.h>
#include <dr_flac.h>
#include <dr_wav.h>

#include "AssetCategory.h"

class AssetLoadingThreadPool;

class AudioProcessor {
 public:
  struct ThreadData {
    ThreadData() = default;
    void Init(int channels, int sampleRate);
    ~ThreadData();

    ogg_stream_state os;
    vorbis_dsp_state vd;
    vorbis_block vb;
    ogg_packet op;
    ogg_page og;
    vorbis_info vi;
    vorbis_comment vc;
  };

  AudioProcessor(bool encode, const std::filesystem::path& asset_destination,
                 const std::filesystem::path& user_asset_root_dir,
                 AssetLoadingThreadPool* thread_pool);

  void Process(const std::filesystem::path& model_path,
               AssetCategory category);

 private:
  struct ThreadBufferData {
    std::unique_ptr<char[]> data;
    long size;
  };

  void EncodeMusic(const std::filesystem::path& audio_path);
  void EncodeSound(const std::filesystem::path& audio_path);

  void DecompressMp3Chunk(drmp3& drmp3_context, float* pcm_data,
                          std::size_t buffer_size, uint64_t* frames);
  void DecompressFlacChunk(drflac& drflac_context, float* pcm_data,
                           std::size_t buffer_size, uint64_t* frames);

  void CompressChunk(float* pcm_data, uint64_t frames,
                     int channels, int sample_rate);

  void WriteCompressedOggChunks(int thread_offset);
  void PrepareOggCompressionContexts(int channels, int sample_rate,
                                     int thread_offset);

  // should be called before each new file
  void PrepareThreadInfo(int channels, int sample_rate);

  void PrepareCompressedOggDataFile(const std::string& to_where);

  void InitThreadBuffers();

  std::filesystem::path asset_destination_;

  // we keep it separate because thread_data_ has Ogg/Vorbis data
  // which are different for each file; thread_buffers_ are the same
  std::vector<AudioProcessor::ThreadData> thread_info_;


  std::vector<ThreadBufferData> thread_buffers_;
  bool thread_buffers_initialized_ = false;

  std::filesystem::path user_asset_root_dir_;

  AssetLoadingThreadPool* thread_pool_ = nullptr;
  long int last_gramulepos_ = 0;

  int thread_number_;

  std::ofstream cur_file_;

  bool encode_;
};

#endif  // ASSETPROCESSOR_AUDIOPROCESSOR_H
