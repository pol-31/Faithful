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
//      alDeleteSources(1, &source);
//      alDeleteBuffers(1, &buffer);
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

// TODO: we need std::unique_ptr binary with data


} // namespace faithful


#endif  // FAITHFUL_SRC_LOADER_SOUND_H_
