#ifndef FAITHFUL_SRC_LOADER_TEXTURE1DPOOL_H_
#define FAITHFUL_SRC_LOADER_TEXTURE1DPOOL_H_

#include <string>

#include "IAssetPool.h"
#include "AssetInstanceInfo.h"
#include "../config/Loader.h"

namespace faithful {
namespace details {
namespace assets {

/// should be only 1 instance for the entire program
class Texture1DPool : public IAssetPool<
                          faithful::config::max_active_texture1d_num> {
 public:
  using Base = IAssetPool<faithful::config::max_active_texture1d_num>;

  Texture1DPool();
  ~Texture1DPool();

  /// not copyable
  Texture1DPool(const Texture1DPool&) = delete;
  Texture1DPool& operator=(const Texture1DPool&) = delete;

  /// movable
  Texture1DPool(Texture1DPool&&) = default;
  Texture1DPool& operator=(Texture1DPool&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  AssetInstanceInfo Load(std::size_t size, float* data, int channels = 4);

 private:
  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTURE1DPOOL_H_
