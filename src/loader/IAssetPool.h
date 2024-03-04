#ifndef FAITHFUL_SRC_LOADER_IASSETPOOL_H_
#define FAITHFUL_SRC_LOADER_IASSETPOOL_H_

#include <iostream> // TODO: replace by logger
#include <string>
#include <vector>
#include <algorithm>

#include "../../utils/CacheBuffer.h"

#include <memory>

#include <type_traits>

namespace faithful {
namespace details {
namespace assets {

template <typename T, int cache_size>
class IAssetPool {
 public:
  static_assert(cache_size >= 0, "cache_size must be non-negative");
  using DataType = std::shared_ptr<T>;

  struct TrackedDataType {
    DataType data;
    std::string path;
  };
  using ActiveDataType = std::vector<TrackedDataType>;
  using CachedDataType = utils::CachedBuffer<TrackedDataType, cache_size>;

  IAssetPool() = default;

  virtual ~IAssetPool() {
    for (auto& i : active_) {
      if (i.data.use_count() != 1) {
        std::cerr
            << "IAssetPool has been destroyed but some std::shared_ptr"
            << " for AssetBase-s still were active" << std::endl;
      }
    }
  }

  /// not copyable
  IAssetPool(const IAssetPool&) = delete;
  IAssetPool& operator=(const IAssetPool&) = delete;

  /// movable
  IAssetPool(IAssetPool&&) = default;
  IAssetPool& operator=(IAssetPool&&) = default;

  DataType Load(std::string path) {
    int found_id = FindInActive(path);
    if (found_id != -1) {
      return active_[found_id].data;
    } else {
      found_id = FindInCache(path);
      if (found_id != -1) {
        active_.push_back(cached_[found_id]);
        return active_.back().data;
      } else { // TODO: reuse memory from cache (CacheBuffer::Older())
        ClearInactive();
        active_.push_back({{}, path});
        return LoadImpl(active_.back()); // TODO: handle if loading failed
      }
    }
  }

  void ClearCache() {
    cached_.Clear();
  }

  /// clean all inactive from active_ write them to cached_
  /// so to fully deleted unused assets should
  /// call ClearInactive() and only after ClearCache()
  void ClearInactive() {
    for (auto rit = active_.rbegin(); rit != active_.rend(); ++rit) {
      if (rit->data.use_count() == 1) {
        cached_.Add(*rit);
      }
    }
    std::remove_if(active_.begin(), active_.end(), [](TrackedDataType& data) {
      return data.data.use_count() == 1;
    });
  }

 protected:
  virtual DataType LoadImpl(TrackedDataType& instance_info) = 0;

 private:
  int FindInActive(const std::string& path) {
    for (int i = 0; i < active_.size(); ++i) {
      if (active_[i].path == path) {
        return i;
      }
    }
    return -1;
  }

  int FindInCache(const std::string& path) {
    for (int i = 0; i < cached_.Size(); ++i) {
      if (cached_[i].path == path) {
        cached_.RenewEntry(i);
        return i;
      }
    }
    return -1;
  }

  ActiveDataType active_;
  CachedDataType cached_;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_IASSETPOOL_H_
