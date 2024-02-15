#ifndef FAITHFUL_SRC_LOADER_SOUNDPOOL_H_
#define FAITHFUL_SRC_LOADER_SOUNDPOOL_H_

#include <array>
#include <string>

#include <AL/al.h>

#include "IAssetPool.h"
#include "AssetInstanceInfo.h"
#include "../config/Loader.h"
#include "AudioData.h"

namespace faithful {
namespace details {

class AudioThreadPool;

namespace assets {

/// should be only 1 instance for the entire program
class SoundPool : public IAssetPool<faithful::config::max_active_sound_num> {
 public:
  using Base = IAssetPool<faithful::config::max_active_sound_num>;

  SoundPool();
  ~SoundPool() = default;

  /// not copyable
  SoundPool(const SoundPool&) = delete;
  SoundPool& operator=(const SoundPool&) = delete;

  /// movable
  SoundPool(SoundPool&&) = default;
  SoundPool& operator=(SoundPool&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  AssetInstanceInfo Load(std::string&& sound_path);

 protected:
  friend class faithful::details::AudioThreadPool;
  std::array<SoundData, faithful::config::max_active_sound_num> sound_heap_data_;

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

  bool LoadSoundData(int sound_id);
  ALenum DeduceSoundFormat(const WavHeader& header);

  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

}  // namespace assets
}  // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SOUNDPOOL_H_
