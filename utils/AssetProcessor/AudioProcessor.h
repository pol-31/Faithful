#ifndef ASSETPROCESSOR_AUDIOPROCESSOR_H
#define ASSETPROCESSOR_AUDIOPROCESSOR_H

#include <filesystem>
#include <vector>

#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>

#include <dr_mp3.h>
#include <dr_flac.h>
#include <dr_wav.h>

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

  AudioProcessor(const std::filesystem::path& asset_destination,
                 AssetLoadingThreadPool* thread_pool);

  ~AudioProcessor();

  void ProcessMusic(const std::filesystem::path& model_path,
                    const std::filesystem::path& path_suffix);
  void ProcessSound(const std::filesystem::path& model_path,
                    const std::filesystem::path& path_suffix);

 private:
  void DecompressMp3Chunk(drmp3& drmp3_context, float** pPCM,
                          uint64_t* frames);
  void DecompressFlacChunk(drflac& drflac_context, float** pPCM,
                           uint64_t* frames);
  void DecompressWavChunk(drwav& drflac_context, float** pPCM,
                          uint64_t* frames);
  void DecompressOggChunk(const std::filesystem::path& model_path, float** pPCM,
                          uint64_t* frames, int* channels, int* sampleRate);

  void CompressChunk(const std::filesystem::path& model_path, float* pPCM,
                     uint64_t frames, int channels, int sampleRate,
                     std::vector<std::pair<long int, std::unique_ptr<char>>>& buffers);

  std::filesystem::path asset_destination_;

  AssetLoadingThreadPool* thread_pool_ = nullptr;
  /// storing there for convenience
  /// (we could ask thread_pool_ for it each time, but this is better)
  int worker_threads_count_;
  long int last_gramulepos_ = 0;
  std::vector<int>* last_packetno_;
};

#endif  // ASSETPROCESSOR_AUDIOPROCESSOR_H
