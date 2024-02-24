#include "SoundPool.h"

#include <array>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <string>

#include <AL/alext.h>

#include "IAsset.h"

namespace faithful {
namespace details {
namespace assets {

SoundPool::SoundPool() {
  for (int i = 0; i < faithful::config::max_active_sound_num; ++i) {
    active_instances_[i].internal_id = i;
  }
}

/// at return value internal_id coincides external_id for Sound
AssetInstanceInfo SoundPool::Load(std::string&& sound_path) {
  auto [sound_id, ref_counter, is_new_id] = Base::AcquireId(sound_path);
  if (sound_id == -1) {
    return {"", nullptr, default_id_, default_id_};
  } else if (!is_new_id) {
    return {std::move(sound_path), ref_counter, sound_id, sound_id};
  }
  if (!LoadSoundData(sound_id)) {
    return {"", nullptr, default_id_, default_id_};
  }
  return {std::move(sound_path), ref_counter, sound_id, sound_id};
}

bool SoundPool::LoadSoundData(int sound_id) {
  auto& found_data = sound_heap_data_[sound_id];

  auto sound_path = active_instances_[sound_id].path;
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

  if (std::string(header.chunk_id, 4) != "RIFF" || std::string(header.format, 4) != "WAVE") {
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

  found_data.data.reset(new char[header.subchunk2_size]);
  sound_file.read(found_data.data.get(), header.subchunk2_size);
  return true;
}

ALenum SoundPool::DeduceSoundFormat(const WavHeader& header) {
  // Vorbis format use float natively, so load as
  // float to avoid clipping when possible
  bool float_ext_supported = false;
  if (alIsExtensionPresent("AL_EXT_float32")) {
    float_ext_supported = true;
  }

  if (float_ext_supported) {
    if (header.bits_per_sample == 8) {
      if (header.num_channels == 1) {
        return AL_FORMAT_MONO_float32;
      } else if (header.num_channels == 2) {
        return AL_FORMAT_STEREO_float32;
      }
    } else if (header.bits_per_sample == 16) {
      if (header.num_channels == 1) {
        return AL_FORMAT_MONO_float32;
      } else if (header.num_channels == 2) {
        return AL_FORMAT_STEREO_float32;
      }
    }
  } else {
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
    }
  }

  std::cerr
      << "SoundManager::DeduceSoundFormat error: unable to deduce format"
      << std::endl;
  return AL_NONE;
}

} // namespace assets
} // namespace details
} // namespace faithful
