#ifndef ASSETPROCESSOR_AUDIOPROCESSOR_H
#define ASSETPROCESSOR_AUDIOPROCESSOR_H

#include <filesystem>
#include <fstream>
#include <vector>

#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

#include <dr_mp3.h>
#include <dr_flac.h>
#include <dr_wav.h>

#include "AssetLoadingThreadPool.h"
#include "ReplaceRequest.h"

class AudioProcessor {
 public:
  struct ThreadData {
    ThreadData() = default;
    void Init(int channels, int sampleRate, int id);
    ~ThreadData();

    ogg_stream_state os;
    vorbis_dsp_state vd;
    vorbis_block vb;
    ogg_packet op;
    ogg_page og;
    vorbis_info vi;
    vorbis_comment vc;
  };

  AudioProcessor() = delete;
  AudioProcessor(AssetLoadingThreadPool& thread_pool,
                 ReplaceRequest& replace_request);

  void Encode(const std::filesystem::path& path);
  void Decode(const std::filesystem::path& path);

  void SetDestinationDirectory(const std::filesystem::path& path);

 private:
  enum class AudioFormat {
    kFlac,
    kMp3,
    kOgg,
    kWav
  };
  enum class AudioSize {
    kMusic,
    kSound
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

  // should be called before each new file
  void PrepareThreadInfo(int channels, int sample_rate);

  std::filesystem::path asset_destination_;

  // we keep it separate because thread_data_ has Ogg/Vorbis data
  // which are different for each file; thread_buffers_ are the same
  std::vector<AudioProcessor::ThreadData> thread_info_;

  std::filesystem::path user_asset_root_dir_;

  AssetLoadingThreadPool& thread_pool_;
  int thread_number_;
  ReplaceRequest& replace_request_;

  std::ofstream cur_file_;

  bool encode_;

  std::filesystem::path sounds_destination_path_;
  std::filesystem::path music_destination_path_;
};

#endif  // ASSETPROCESSOR_AUDIOPROCESSOR_H
