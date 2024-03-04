#ifndef FAITHFUL_SRC_LOADER_ASSETBASE_H_
#define FAITHFUL_SRC_LOADER_ASSETBASE_H_

#include <memory>

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

 protected:
  std::shared_ptr<T> data_;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETBASE_H_
