#ifndef FAITHFUL_SRC_LOADER_AUDIO_H_
#define FAITHFUL_SRC_LOADER_AUDIO_H_

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <string>
#include <vector>

#include "../config/AssetFormats.h"

#include "IAsset.h"

namespace faithful {

namespace details {
namespace audio {

/// should be only 1 instance for the entire program
template <int max_active_music> // 2 by default
class MusicManager
    : public faithful::details::IAssetManager<max_active_music> {
 public:
  using Base = faithful::details::IAssetManager<max_active_music>;
  using InstanceInfo = details::InstanceInfo;

  enum class TextureCategory {
    kLdr,
    kHdr,
    kNmap
  };
  MusicManager() {
    // TODO:
    //  1) call glGenTextures for __num__ textures
    //  2) load 1 default texture
    //  3) init free_instances_ with indices: 1,2,3,....,max_active_texture_num
  }

  ~MusicManager() {
    for (auto& i : active_instances_) {
      // TODO: remove source
      //      alDeleteSources(1, &source);
      //      alDeleteBuffers(1, &buffer);
    }
  }

  /// not copyable
  MusicManager(const MusicManager&) = delete;
  MusicManager& operator=(const MusicManager&) = delete;

  /// movable
  MusicManager(MusicManager&&) = default;
  MusicManager& operator=(MusicManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& music_path) {
    for (auto& t : active_instances_) {
      if (t.path == music_path) {
        ++t.ref_counter;
        return t.opengl_id;
      }
    }
    if (!Base::CleanInactive()) {
      std::cerr << "Can't load texture: reserve more place for textures; "
//                << max_active_texture_num << " is not enough;\n"
                << "failure for: " << music_path << std::endl;
      return default_music_id_;
    }
    int id = free_instances_.Back();
    free_instances_.PopBack();
    auto& instance = active_instances_[id];
    ++instance.ref_counter;
    instance.path = std::move(music_path);
    LoadTextureData(id);
    return id;
  }

 private:

  void LoadTextureData(int active_instance_id) {
    auto& instance = active_instances_[active_instance_id];
    /// assets/textures contain only 6x6x1 astc, so there's no need
    /// to check is for "ASTC" and block size.
    /// but we still need to get texture resolution
    std::ifstream texture_file(instance.path);
  }

  using Base::active_instances_;
  using Base::free_instances_;
  int default_music_id_ = 0;  // adjust
};

}  // namespace audio
}  // namespace details

class Music : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

  void Play() {
    //
  }

  void PlayForce() {
    //
  }

 private:
  using Base::opengl_id_;
};


} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_AUDIO_H_
