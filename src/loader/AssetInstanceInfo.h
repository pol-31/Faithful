#ifndef FAITHFUL_SRC_LOADER_ASSETINSTANCEINFO_H_
#define FAITHFUL_SRC_LOADER_ASSETINSTANCEINFO_H_

namespace faithful {
namespace details {
namespace assets {

template <typename T>
struct AssetInstanceInfo {
  std::shared_ptr<T> data;
  int id;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETINSTANCEINFO_H_
