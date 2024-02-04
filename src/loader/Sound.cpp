#include "Sound.h"

#include <array>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <string>

#include <AL/al.h>

#include "AudioData.h"
#include "IAsset.h"
#include "../config/Loader.h"

namespace faithful {
namespace details {
namespace audio {

std::array<SoundData, faithful::config::max_active_sound_num> sound_heap_data_;


SoundManager::SoundManager() {
  for (int i = 0; i < faithful::config::max_active_sound_num; ++i) {
    free_instances_[i] = i;
  }
}

SoundManager::~SoundManager() {
  for (auto& i : active_instances_) {
    if (i.ref_counter->Active()) {
      std::cerr
          << "SoundManager deleted, but ref_counter is still active for "
          << i.opengl_id << std::endl;
      // TODO: terminate?
    }
    delete i.ref_counter;
  }
}

InstanceInfo SoundManager::Load(std::string&& sound_path) {
  auto [sound_id, ref_counter, is_new_id] = Base::AcquireId(sound_path);
  if (sound_id == 0) {
    return {"", nullptr, default_sound_id_};
  } else if (!is_new_id) {
    return {std::move(sound_path), ref_counter, sound_id};
  }
  if (!LoadSoundData(sound_id, sound_path)) {
    return {"", nullptr, default_sound_id_};
  }
  return {std::move(sound_path), ref_counter, sound_id};
}

bool SoundManager::LoadSoundData(int sound_id, const std::string& sound_path) {
  auto& found_data = sound_heap_data_[sound_id];

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

  sound_file.read(found_data.data.get(), header.subchunk2_size);
  return true;
}

ALenum SoundManager::DeduceSoundFormat(const WavHeader& header) {
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



} // namespace faithful
} // namespace details
} // namespace audio
