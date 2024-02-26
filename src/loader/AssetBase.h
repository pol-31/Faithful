#ifndef FAITHFUL_SRC_LOADER_ASSETBASE_H_
#define FAITHFUL_SRC_LOADER_ASSETBASE_H_

#include "AssetInstanceInfo.h"

namespace faithful {
namespace details {
namespace assets {

template <typename T>
class AssetBase {
 public:
  AssetBase() = default;

  AssetBase(const AssetBase& other) = default;
  AssetBase& operator=(const AssetBase& other) = default;

  AssetBase(AssetBase&& other) = default;
  AssetBase& operator=(AssetBase&& other) = default;

  AssetBase(std::shared_ptr<T> data) : data_(data) {}

  typename std::remove_extent_t<T>& GetData() {
    return *data_.data;
  }
  const typename std::remove_extent_t<T>& GetData() const {
    return *data_.data;
  }

  int GetId() const {
    return data_.id;
  }

 protected:
  AssetInstanceInfo<T> data_;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETBASE_H_
