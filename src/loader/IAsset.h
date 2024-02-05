#ifndef FAITHFUL_SRC_LOADER_IASSET_H_
#define FAITHFUL_SRC_LOADER_IASSET_H_

#include <array>
#include <iostream> // TODO: replace by utils/Logger.h
#include <tuple>

#include <glad/glad.h>

#include "../utils/ConstexprVector.h"


// TODO: rename file / OR relocate IAsset to separate header


namespace faithful {
namespace details {

class AssetManagerRefCounter {
 public:
  AssetManagerRefCounter() : counter_(0) {}

  /// not copyable
  AssetManagerRefCounter(const AssetManagerRefCounter&) = delete;
  AssetManagerRefCounter& operator=(const AssetManagerRefCounter&) = delete;

  /// movable
  AssetManagerRefCounter(AssetManagerRefCounter&&) = default;
  AssetManagerRefCounter& operator=(AssetManagerRefCounter&&) = default;

  void Increment() {
    ++counter_;
  }

  void Decrement() {
    if (counter_ == 0) {
      // TODO: log <- warning
    } else {
      --counter_;
    }
  }

  bool Active() {
    return counter_ != 0;
  }

 private:
  int counter_;
};


struct InstanceInfo {
  InstanceInfo() : ref_counter(new AssetManagerRefCounter) {};
  InstanceInfo(std::string&& src_path, AssetManagerRefCounter* ref_cnt, int id)
      : path(src_path), ref_counter(ref_cnt), opengl_id(id) {};
  std::string path{}; // TODO: does _hash() make sense?
  AssetManagerRefCounter* ref_counter;

  /// for OpenGL objects (shaders, textures) data is stored by OpenGl,
  /// so our id directly translated into internal OpenGL ids;
  /// for audio - data stored on the HEAP by (e.g.) SoundManager::data_ std::map
  int opengl_id = -1; // TODO: not only OpenGL (see Sound.h, Music.h)
};

/** Base class for ShaderProgramManager, ShaderObjectManager, TextureManager, etc
 * despite 'I' that's not interface, but still shouldn't be used directly
 * each Derived_class should implement Load();
 * */
template <int max_instances>
class IAssetManager {
 public:
  IAssetManager() = default;

  /// not copyable
  IAssetManager(const IAssetManager&) = delete;
  IAssetManager& operator=(const IAssetManager&) = delete;

  /// movable
  IAssetManager(IAssetManager&&) = default;
  IAssetManager& operator=(IAssetManager&&) = default;


  /// we don't need virtual functions, so just hide it from Derived class
  /// (there's no usage of this dynamic polymorphism on Faithful by now)
  InstanceInfo Load(std::string&&) {
    HandleMissingImpl();
  }

  bool CleanInactive() {
    if (free_instances_.Size() == 0) {
      for (auto& t: active_instances_) {
        if (!t.ref_counter->Active()) {
          t.path.clear();
          /// safe because both std::array have the same size
          free_instances_[free_instances_.Size()];
        }
      }
    }
    return free_instances_.Size() == 0 ? false : true;
  }

  /// returns opengl id, active_instances id, is "new" (object new, id reused)
  std::tuple<int, AssetManagerRefCounter*, bool> AcquireId(const std::string& path) {
    for (int i = 0; i < active_instances_.size(); ++i) {
      if (active_instances_[i].path == path) {
//        ++active_instances_[i].ref_counter;
        return {active_instances_[i].opengl_id,
                active_instances_[i].ref_counter,
                false}; // "new" object
      }
    }
    if (free_instances_.Empty()) {
      if (!CleanInactive()) {
        return {-1, nullptr, false};
      }
    }
    int active_instances_id = free_instances_.Back();
    free_instances_.PopBack();
    active_instances_[active_instances_id].path = path;
    return {active_instances_[active_instances_id].opengl_id,
            active_instances_[active_instances_id].ref_counter,
            true};
  }

 protected:
  std::array<InstanceInfo, max_instances> active_instances_;
  faithful::utils::ConstexprVector<int, max_instances> free_instances_; // id's for active_instances TODO: explain

 private:
  // TODO: static assert
  void HandleMissingImpl() {
    std::cerr << "Derived of IResourceManager must implement "
              << "Load()" << std::endl;
  }
};

// TODO: explain opengl_id_, etc, etc
class IAsset {
 public:
  using InstanceInfo = faithful::details::InstanceInfo;

  IAsset() {
    ref_counter_ = nullptr;
    opengl_id_ = -1;
  }

  IAsset(const InstanceInfo& instance_info) {
    opengl_id_ = instance_info.opengl_id;
    if (ref_counter_) {
      ref_counter_ = &*instance_info.ref_counter;
      ref_counter_->Increment();
    }
  }

  IAsset(const IAsset& other) {
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    ref_counter_->Increment();
  }
  IAsset(IAsset&& other) {
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    other.ref_counter_ = nullptr;
  }

  IAsset& operator=(const IAsset& other) {
    if (other == *this) {
      return *this;
    }
    DetachRefCounter();
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    ref_counter_->Increment();
    return *this;
  }
  IAsset& operator=(IAsset&& other) {
    if (other == *this) {
      return *this;
    }
    DetachRefCounter();
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    other.ref_counter_ = nullptr;
    return *this;
  }

  ~IAsset() {
    DetachRefCounter();
  }

  int OpenglId() const {
    return opengl_id_;
  }

  int GlobalId() const {
    return global_id_;
  }

  friend bool operator==(const IAsset& tex1, const IAsset& tex2) {
    return tex1.global_id_ == tex2.global_id_;
  }

  // TODO: explain SetRefCounter & SetOpenglId

  /// DON'T USE without ref_counter handling)
  void SetRefCounter(details::AssetManagerRefCounter* ref_counter) {
    ref_counter_ = &*ref_counter;
  }
  /// DON'T USE without ref_counter handling)
  void SetOpenglId(int id) {
    opengl_id_ = id;
  }

 protected:
  details::AssetManagerRefCounter* ref_counter_;
  int opengl_id_;
  int global_id_;

 private:
  void DetachRefCounter() {
    if (ref_counter_) {
      ref_counter_->Decrement();
    }
  }
};


} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_IASSET_H_
