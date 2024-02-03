#ifndef FAITHFUL_SRC_LOADER_SOUND_H_
#define FAITHFUL_SRC_LOADER_SOUND_H_

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <map>
#include <string>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>

#include "IAsset.h"
#include "../config/AssetFormats.h"
#include "AudioData.h"

namespace faithful {

namespace details {
namespace audio {

/// should be only 1 instance for the entire program
template <int max_active_sounds> // 4 by default
class SoundManager
    : public faithful::details::IAssetManager<max_active_sounds> {
 public:
  using Base = faithful::details::IAssetManager<max_active_sounds>;
  using InstanceInfo = details::InstanceInfo;

  SoundManager() {
    for (int i = 0; i < max_active_sounds; ++i) {
      active_instances_[i] = i;
      free_instances_[i] = i;
      data_[i] = {}; // TODO: optimize num of allocations
    }
  }

  ~SoundManager() {
    for (auto& i : active_instances_) {
      if (i.ref_counter.Active()) {
        std::cerr
            << "MusicManager deleted, but ref_counter is still active for "
            << i.opengl_id << std::endl;
        // TODO: terminate?
      }
      delete i.ref_counter;
    }
  }

  /// not copyable
  SoundManager(const SoundManager&) = delete;
  SoundManager& operator=(const SoundManager&) = delete;

  /// movable
  SoundManager(SoundManager&&) = default;
  SoundManager& operator=(SoundManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& sound_path) {
    auto [sound_id, active_instances_id, is_new_id] = Base::AcquireId(sound_path); // structure binding
    if (sound_id == 0) {
      return {"", nullptr, default_sound_id_};
    } else if (!is_new_id) {
      return {sound_path, active_instances_[active_instances_id], sound_id};
    }
    if (!LoadSoundData(sound_id, sound_path)) {
      return {"", nullptr, default_sound_id_};
    }
    return {sound_path, active_instances_[active_instances_id], sound_id};
  }

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

  bool LoadSoundData(int sound_id, const std::string& sound_path) {
    auto found_element = data_.find(sound_id);
    if (found_element == data_.end()) {
      std::cerr << "SoundManager::data_ error: can't find id in LoadSoundData()"
                << std::endl;
      return false;
    }

    auto& found_data = found_element->second;

    std::ifstream sound_file(sound_path, std::ios::binary);
    if (!sound_file.is_open()) {
      std::cerr << "Error: unable to open file: " << sound_path << std::endl;
      return false;
    }

    WavHeader header;
    sound_file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

    if (sound_file.gcount() != sizeof(WavHeader)) {
      std::cerr << "Error: unable to read WAV header from file: " << sound_path << std::endl;
      return false;
    }

    if (std::string(header.chunkId, 4) != "RIFF" || std::string(header.format, 4) != "WAVE") {
      std::cerr << "Error: not a valid WAV file: " << sound_path << std::endl;
      return false;
    }

    found_data.filename = sound_path;
    found_data.channels = header.num_channels;
    found_data.sample_rate = header.sample_rate;
    found_data.bits_per_sample = header.bits_per_sample;
    found_data.size = header.subchunk2_size;

    ALenum format = DeduceSoundFormat(header);
    if (format == AL_NONE) {
      return false;
    }
    found_data.format = format;

    sound_file.read(found_data.data, header.subchunk2_size);
    return true;
  }

  ALenum DeduceSoundFormat(const WavHeader& header) {
    if (header.bits_per_sample == 8) {
      if (header.num_channels == 1) {
        return AL_FORMAT_MONO8;
      } else if (header.num_channels == 2) {
        return AL_FORMAT_STEREO8;
      }
    } else if (header.bits_per_sample == 16) {
      if (header.num_channels == 1) {
        return AL_FORMAT_MONO16;
      } else if (header.num_channels == 2) {
        return AL_FORMAT_STEREO16;
      }
    } else {
      std::cerr
          << "SoundManager::DeduceSoundFormat error: unable to deduce format"
          << std::endl;
      return AL_NONE;
    }
  }

  std::map<int, SoundData> data_;

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

  // TODO: add protected section; friend AudioThreadPool; protected SoundData
 private:
  using Base::opengl_id_;
};

// TODO: we need std::unique_ptr binary with data


} // namespace faithful


#endif  // FAITHFUL_SRC_LOADER_SOUND_H_
