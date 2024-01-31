#ifndef FAITHFUL_SRC_LOADER_SOUND_H_
#define FAITHFUL_SRC_LOADER_SOUND_H_


#include <array>
#include <filesystem>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <string>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>

#include <vorbis/vorbisfile.h>
#include <cstring>


#include "../config/AssetFormats.h"

#include "IAsset.h"

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
    // TODO:
    //  1) gen max_active_sounds OpenAL sources
    //  2) load 1 default sound
    //  3) init free_instances_ with indices: 1,2,3,....,max_active_sounds_num
  }

  ~SoundManager() {
    for (auto& i : active_instances_) {
      // TODO: remove source
    }
  }

  /// not copyable
  SoundManager(const SoundManager&) = delete;
  SoundManager& operator=(const SoundManager&) = delete;

  /// movable
  SoundManager(SoundManager&&) = default;
  SoundManager& operator=(SoundManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  int Load(std::string&& sound_path) {
    for (auto& t : active_instances_) {
      if (t.path == sound_path) {
        ++t.ref_counter;
        return t.opengl_id;
      }
    }
    if (!Base::CleanInactive()) {
      std::cerr << "Can't load texture: reserve more place for sounds; "
                //                << max_active_texture_num << " is not enough;\n"
                << "failure for: " << sound_path << std::endl;
      return default_sound_id_;
    }
    int id = free_instances_.Back();
    free_instances_.PopBack();
    auto& instance = active_instances_[id];
    ++instance.ref_counter;
    instance.path = std::move(sound_path);
    LoadSoundData(id);
    return id;
  }

 private:

  void LoadSoundData(int active_instance_id) {
    auto& instance = active_instances_[active_instance_id];
    /// assets/textures contain only 6x6x1 astc, so there's no need
    /// to check is for "ASTC" and block size.
    /// but we still need to get texture resolution
    std::ifstream texture_file(instance.path);
  }

  bool create_stream_from_file(const std::string& filename,
                               StreamingAudioData& audioData) {
    audioData.filename = filename;
    audioData.file.open(filename, std::ios::binary);
    if(!audioData.file.is_open())
    {
      std::cerr << "ERROR: couldn't open file" << std::endl;
      return 0;
    }

    audioData.file.seekg(0, std::ios_base::beg);
    audioData.file.ignore(std::numeric_limits<std::streamsize>::max());
    audioData.size = audioData.file.gcount();
    audioData.file.clear();
    audioData.file.seekg(0,std::ios_base::beg);
    audioData.sizeConsumed = 0;

    ov_callbacks oggCallbacks;
    oggCallbacks.read_func = read_ogg_callback;
    oggCallbacks.close_func = nullptr;
    oggCallbacks.seek_func = seek_ogg_callback;
    oggCallbacks.tell_func = tell_ogg_callback;

    if(ov_open_callbacks(reinterpret_cast<void*>(&audioData),
                          &audioData.oggVorbisFile, nullptr, -1,
                          oggCallbacks) < 0)
    {
      std::cerr << "ERROR: Could not ov_open_callbacks" << std::endl;
      return false;
    }

    vorbis_info* vorbisInfo = ov_info(&audioData.oggVorbisFile, -1);

    audioData.channels = vorbisInfo->channels;
    audioData.bitsPerSample = 16;
    audioData.sampleRate = vorbisInfo->rate;
    audioData.duration = ov_time_total(&audioData.oggVorbisFile, -1);

    alCall(alGenSources, 1, &audioData.source);
    alCall(alSourcef, audioData.source, AL_PITCH, 1);
    alCall(alSourcef, audioData.source, AL_GAIN, 0.5);//DEFAULT_GAIN);
    alCall(alSource3f, audioData.source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, audioData.source, AL_VELOCITY, 0, 0, 0);
    alCall(alSourcei, audioData.source, AL_LOOPING, AL_FALSE);

    alCall(alGenBuffers, NUM_BUFFERS, &audioData.buffers[0]);

    if(audioData.file.eof())
    {
      std::cerr << "ERROR: Already reached EOF without loading data" << std::endl;
      return false;
    }
    else if(audioData.file.fail())
    {
      std::cerr << "ERROR: Fail bit set" << std::endl;
      return false;
    }
    else if(!audioData.file)
    {
      std::cerr << "ERROR: file is false" << std::endl;
      return false;
    }

    char* data = new char[BUFFER_SIZE];

    for(std::uint8_t i = 0; i < NUM_BUFFERS; ++i)
    {
      std::int32_t dataSoFar = 0;
      while(dataSoFar < BUFFER_SIZE)
      {
        std::int32_t result = ov_read(&audioData.oggVorbisFile, &data[dataSoFar], BUFFER_SIZE - dataSoFar, 0, 2, 1, reinterpret_cast<int*>(&audioData.oggCurrentSection));
        if(result == OV_HOLE)
        {
          std::cerr << "ERROR: OV_HOLE found in initial read of buffer " << i << std::endl;
          break;
        }
        else if(result == OV_EBADLINK)
        {
          std::cerr << "ERROR: OV_EBADLINK found in initial read of buffer " << i << std::endl;
          break;
        }
        else if(result == OV_EINVAL)
        {
          std::cerr << "ERROR: OV_EINVAL found in initial read of buffer " << i << std::endl;
          break;
        }
        else if(result == 0)
        {
          std::cerr << "ERROR: EOF found in initial read of buffer " << i << std::endl;
          break;
        }

        dataSoFar += result;
      }

      if(audioData.channels == 1 && audioData.bitsPerSample == 8)
        audioData.format = AL_FORMAT_MONO8;
      else if(audioData.channels == 1 && audioData.bitsPerSample == 16)
        audioData.format = AL_FORMAT_MONO16;
      else if(audioData.channels == 2 && audioData.bitsPerSample == 8)
        audioData.format = AL_FORMAT_STEREO8;
      else if(audioData.channels == 2 && audioData.bitsPerSample == 16)
        audioData.format = AL_FORMAT_STEREO16;
      else
      {
        std::cerr << "ERROR: unrecognised ogg format: " << audioData.channels << " channels, " << audioData.bitsPerSample << " bps" << std::endl;
        delete[] data;
        return false;
      }

      alCall(alBufferData, audioData.buffers[i], audioData.format, data, dataSoFar, audioData.sampleRate);
    }

    alCall(alSourceQueueBuffers, audioData.source, NUM_BUFFERS, &audioData.buffers[0]);

    delete[] data;

    return true;
  }


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

    //  void Bind(GLenum target);

 private:
  using Base::opengl_id_;
};


} // namespace faithful


#endif  // FAITHFUL_SRC_LOADER_SOUND_H_