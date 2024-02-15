#ifndef FAITHFUL_SRC_LOADER_IASSETPOOL_H_
#define FAITHFUL_SRC_LOADER_IASSETPOOL_H_

#include <array>
#include <iostream> // TODO: replace by utils/Logger.h
#include <string>
#include <tuple>

#include "AssetInstanceInfo.h"

#include "../utils/ConstexprVector.h"
#include "../common/RefCounter.h"

namespace faithful {
namespace details {
namespace assets {

/** Base class for ShaderProgramManager, ShaderObjectManager, TextureManager, etc
 * despite 'I' that's not interface, but still shouldn't be used directly
 * each Derived_class should implement Load();
 * */
template <int max_instances>
class IAssetPool {
 public:
  IAssetPool() {
    for (int i = 0; i < max_instances; ++i) {
      free_instances_.PushBack(i);
    }
  };

  ~IAssetPool() {
    for (auto& i : active_instances_) {
      if (i.ref_counter->Active()) {
        std::cerr
            << "IAssetManager has been deleted but some ref_counters"
            << " for IAsset-s still were active" << std::endl;
      }
      delete i.ref_counter;
    }
  }

  /// not copyable
  IAssetPool(const IAssetPool&) = delete;
  IAssetPool& operator=(const IAssetPool&) = delete;

  /// movable
  IAssetPool(IAssetPool&&) = default;
  IAssetPool& operator=(IAssetPool&&) = default;

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
  std::tuple<int, RefCounter*, bool> AcquireId(const std::string& path) {
    if (!path.empty()) {
      for (int i = 0; i < active_instances_.size(); ++i) {
        if (active_instances_[i].path == path) {
          //        ++active_instances_[i].ref_counter;
          return {active_instances_[i].internal_id,
                  active_instances_[i].ref_counter,
                  false}; // "new" object
        }
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
    return {active_instances_[active_instances_id].internal_id,
            active_instances_[active_instances_id].ref_counter,
            true};
  }

 protected:
  std::array<AssetInstanceInfo, max_instances> active_instances_;
  // id's for active_instances TODO: explain
  faithful::utils::ConstexprVector<int, max_instances> free_instances_;
  int default_id_ = 0;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_IASSETPOOL_H_
