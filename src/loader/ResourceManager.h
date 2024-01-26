#ifndef FAITHFUL_SRC_LOADER_RESOURCEMANAGER_H_
#define FAITHFUL_SRC_LOADER_RESOURCEMANAGER_H_

#include <array>

// TODO: replace by utils/Logger.h
#include <iostream>

#include "../utils/ConstexprVector.h"

namespace faithful {
namespace details {

class ResourceManagerRefCounter {
 public:
  ResourceManagerRefCounter() : counter_(0) {}

  /// not copyable
  ResourceManagerRefCounter(const ResourceManagerRefCounter&) = delete;
  ResourceManagerRefCounter& operator=(const ResourceManagerRefCounter&) = delete;

  /// movable
  ResourceManagerRefCounter(ResourceManagerRefCounter&&) = default;
  ResourceManagerRefCounter& operator=(ResourceManagerRefCounter&&) = default;

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

/** Base class for ShaderProgramManager, ShaderObjectManager, TextureManager, etc
 * despite 'I' that's not interface, but still shouldn't be used directly
 * each Derived_class should implement Load();
 * */
template <int max_instances>
class IResourceManager {
 public:
  struct InstanceInfo {
    InstanceInfo() {
      ref_counter = new ResourceManagerRefCounter;
    }
    std::string* path = nullptr; // TODO: does _hash() make sense?
    ResourceManagerRefCounter* ref_counter;
    int opengl_id = -1;
  };

  IResourceManager() = default;

  /// not copyable
  IResourceManager(const IResourceManager&) = delete;
  IResourceManager& operator=(const IResourceManager&) = delete;

  /// movable
  IResourceManager(IResourceManager&&) = default;
  IResourceManager& operator=(IResourceManager&&) = default;


  /// we don't need virtual functions, so just hide it from Derived class
  /// (there's no usage of this dynamic polymorphism on Faithful by now)
  InstanceInfo Load() {
    HandleMissingImpl();
  }

  // TODO: these:
  //  - void Restore(int opengl_id)
  //  - void ReuseId(int opengl_id)
  //  should be implemented directly inside the Texture, ShaderObject
  //  classes because its just effective (they have ptr to ref_counter)
  //  So they should handle both incrementing / decrementing
  //  So need somehow always remember about it <------------------------

  bool CleanInactive() {
    if (free_instances_.Size() == 0) {
      for (auto& t: active_instances_) {
        if (!t.ref_counter->Active()) {
          delete t.path;
          /// safe because both std::array have the same size
          free_instances_[free_instances_.Size()];
        }
      }
    }
    return free_instances_.Size() == 0 ? false : true;
  }

 protected:
  std::array<InstanceInfo, max_instances> active_instances_;
  faithful::utils::ConstexprVector<int, max_instances> free_instances_;

 private:
  // TODO: static assert
  void HandleMissingImpl() {
    std::cerr << "Derived of IResourceManager must implement "
              << "Load()" << std::endl;
  }
};

// TODO: explain opengl_id_, etc, etc
class IResource {
 public:
  using InstanceInfo = faithful::details::IResourceManager::InstanceInfo;

  IResource() {
    ref_counter_ = nullptr;
    opengl_id_ = -1;
  }

  IResource(const InstanceInfo& instance_info) {
    opengl_id_ = instance_info.opengl_id;
    ref_counter_ = &*instance_info.ref_counter;
  }

  IResource(const IResource& other) {
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    ref_counter_->Increment();
  }
  IResource(IResource&& other) {
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    other.ref_counter_ = nullptr;
  }

  IResource& operator=(const IResource& other) {
    if (other == *this) {
      return *this;
    }
    DetachRefCounter();
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    ref_counter_->Increment();
    return *this;
  }
  IResource& operator=(IResource&& other) {
    if (other == *this) {
      return *this;
    }
    DetachRefCounter();
    opengl_id_ = other.opengl_id_;
    ref_counter_ = &*other.ref_counter_;
    other.ref_counter_ = nullptr;
    return *this;
  }

  ~IResource() {
    DetachRefCounter();
  }

  // TODO: should be hided in derived class
  void Bind(GLenum target);

  friend bool operator==(const IResource& tex1, const IResource& tex2) {
    return tex1.global_id_ == tex2.global_id_;
  }

  // TODO: explain

  /// DON'T USE without ref_counter handling)
  void SetRefCounter(details::ResourceManagerRefCounter* ref_counter) {
    ref_counter_ = &*ref_counter;
  }
  /// DON'T USE without ref_counter handling)
  void SetOpenglId(int id) {
    opengl_id_ = id;
  }
 private:
  void DetachRefCounter() {
    if (ref_counter_) {
      ref_counter_->Decrement();
    }
  }

  details::ResourceManagerRefCounter* ref_counter_;
  int opengl_id_;
  int global_id_;
};


} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_RESOURCEMANAGER_H_
