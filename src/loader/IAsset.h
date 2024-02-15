#ifndef FAITHFUL_SRC_LOADER_IASSET_H_
#define FAITHFUL_SRC_LOADER_IASSET_H_

#include "AssetInstanceInfo.h"
#include "../common/RefCounter.h"

namespace faithful {
namespace details {
namespace assets {

class IAsset {
 public:
  IAsset() {
    ref_counter_ = nullptr;
    internal_id_ = -1;
  }

  IAsset(const AssetInstanceInfo& instance_info) {
    internal_id_ = instance_info.internal_id;
    if (ref_counter_) {
      ref_counter_ = &*instance_info.ref_counter;
      ref_counter_->Increment();
    }
  }

  IAsset(const IAsset& other) {
    internal_id_ = other.internal_id_;
    ref_counter_ = &*other.ref_counter_;
    ref_counter_->Increment();
  }
  IAsset(IAsset&& other) {
    internal_id_ = other.internal_id_;
    ref_counter_ = &*other.ref_counter_;
    other.ref_counter_ = nullptr;
  }

  IAsset& operator=(const IAsset& other) {
    if (other == *this) {
      return *this;
    }
    DetachRefCounter();
    internal_id_ = other.internal_id_;
    ref_counter_ = &*other.ref_counter_;
    ref_counter_->Increment();
    return *this;
  }
  IAsset& operator=(IAsset&& other) {
    if (other == *this) {
      return *this;
    }
    DetachRefCounter();
    internal_id_ = other.internal_id_;
    ref_counter_ = &*other.ref_counter_;
    other.ref_counter_ = nullptr;
    return *this;
  }

  ~IAsset() {
    DetachRefCounter();
  }

  int GetInternalId() const {
    return internal_id_;
  }

  int GetExternalId() const {
    return external_id_;
  }

  friend bool operator==(const IAsset& tex1, const IAsset& tex2) {
    return tex1.external_id_ == tex2.external_id_;
  }

 protected:
  details::RefCounter* ref_counter_;
  int internal_id_; // see explanation in struct InstanceInfo
  int external_id_; // simply saying it's idx in active_assets_

 private:
  void DetachRefCounter() {
    if (ref_counter_) {
      ref_counter_->Decrement();
    }
  }
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_IASSET_H_
