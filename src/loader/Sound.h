#ifndef FAITHFUL_SRC_LOADER_SOUND_H_
#define FAITHFUL_SRC_LOADER_SOUND_H_

#include <array>
#include <string>

#include <AL/al.h>

#include "IAsset.h"
#include "../config/Loader.h"

namespace faithful {

namespace details {

struct SoundData;

namespace audio {

extern std::array<SoundData, faithful::config::max_active_sound_num> sound_heap_data_;

/// should be only 1 instance for the entire program
class SoundManager
    : public faithful::details::IAssetManager<faithful::config::max_active_sound_num> {
 public:
  using Base = faithful::details::IAssetManager<faithful::config::max_active_sound_num>;
  using InstanceInfo = details::InstanceInfo;

  SoundManager();
  ~SoundManager();

  /// not copyable
  SoundManager(const SoundManager&) = delete;
  SoundManager& operator=(const SoundManager&) = delete;

  /// movable
  SoundManager(SoundManager&&) = default;
  SoundManager& operator=(SoundManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& sound_path);

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

  bool LoadSoundData(int sound_id, const std::string& sound_path);
  ALenum DeduceSoundFormat(const WavHeader& header);

  using Base::active_instances_;
  using Base::free_instances_;
  int default_sound_id_ = 0;  // adjust
};

}  // namespace audio
}  // namespace details

class Sound : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

 private:
  using Base::opengl_id_;
};

} // namespace faithful


#endif  // FAITHFUL_SRC_LOADER_SOUND_H_
