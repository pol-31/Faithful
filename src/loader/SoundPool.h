#ifndef FAITHFUL_SRC_LOADER_SOUNDPOOL_H_
#define FAITHFUL_SRC_LOADER_SOUNDPOOL_H_

#include <AL/al.h>

#include "IAssetPool.h"
#include "../../config/Loader.h"
#include "assets_data/SoundData.h"

namespace faithful {
namespace details {

class AudioThreadPool;

namespace assets {

class SoundPool
    : public IAssetPool<SoundData, faithful::config::kSoundCacheSize> {
 public:
  using Base = IAssetPool<SoundData, faithful::config::kSoundCacheSize>;
  using DataType = typename Base::DataType;

  SoundPool() = delete;
  SoundPool(AudioThreadPool* audio_thread_pool);

  /// not copyable
  SoundPool(const SoundPool&) = delete;
  SoundPool& operator=(const SoundPool&) = delete;

  /// movable
  SoundPool(SoundPool&&) = default;
  SoundPool& operator=(SoundPool&&) = default;

 private:
  struct WavHeader {
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char subchunk2_id[4];
    uint32_t subchunk2_size;
  };

  DataType LoadImpl(typename Base::TrackedDataType& instance_info) override;

  ALenum DeduceSoundFormat(const WavHeader& header);

  AudioThreadPool* audio_thread_pool_;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SOUNDPOOL_H_
