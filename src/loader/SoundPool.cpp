#include "SoundPool.h"

#include <fstream>
#include <iostream> // todo: replace by Logger
#include <string>

#include <AL/alext.h>

namespace faithful {
namespace details {
namespace assets {

SoundPool::SoundPool(AudioThreadPool* audio_thread_pool_)
    : audio_thread_pool_(audio_thread_pool_) {}

SoundPool::DataType SoundPool::LoadImpl(
    typename Base::TrackedDataType& instance_info) {
  auto sound_path = instance_info.path;
  std::ifstream sound_file(sound_path, std::ios::binary);
  if (!sound_file.is_open()) {
    std::cerr << "Error: unable to open file: " << sound_path << std::endl;
    std::terminate();
  }

  WavHeader header;
  sound_file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

  if (sound_file.gcount() != sizeof(WavHeader)) {
    std::cerr << "Error: unable to read WAV header from file: "
              << sound_path << std::endl;
    std::terminate();
  }

  if (std::string(header.chunk_id, 4) != "RIFF" ||
      std::string(header.format, 4) != "WAVE") {
    std::cerr << "Error: not a valid WAV file: " << sound_path << std::endl;
    std::terminate();
  }

  instance_info.data->filename = sound_path;
  instance_info.data->channels = header.num_channels;
  instance_info.data->sample_rate = header.sample_rate;
  instance_info.data->bits_per_sample = header.bits_per_sample;
  instance_info.data->size = header.subchunk2_size;

  instance_info.data->audio_thread_pool = audio_thread_pool_;

  ALenum format = DeduceSoundFormat(header);
  if (format == AL_NONE) {
    std::terminate();
  }
  instance_info.data->format = format;

  instance_info.data->data.reset(new char[header.subchunk2_size]);
  sound_file.read(instance_info.data->data.get(), header.subchunk2_size);
  return instance_info.data;
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
        return AL_FORMAT_MONO_FLOAT32;
      } else if (header.num_channels == 2) {
        return AL_FORMAT_STEREO_FLOAT32;
      }
    } else if (header.bits_per_sample == 16) {
      if (header.num_channels == 1) {
        return AL_FORMAT_MONO_FLOAT32;
      } else if (header.num_channels == 2) {
        return AL_FORMAT_STEREO_FLOAT32;
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
