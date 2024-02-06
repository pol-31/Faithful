#ifndef FAITHFUL_SRC_LOADER_TEXTURE1D_H_
#define FAITHFUL_SRC_LOADER_TEXTURE1D_H_

#include <string>

#include "IAsset.h"
#include "../config/Loader.h"

namespace faithful {
namespace details {
namespace texture {

/// should be only 1 instance for the entire program
class Texture1DManager : public faithful::details::IAssetManager<
                             faithful::config::max_active_texture1d_num> {
 public:
  using Base = faithful::details::IAssetManager<
      faithful::config::max_active_texture1d_num>;

  Texture1DManager();
  ~Texture1DManager();

  /// not copyable
  Texture1DManager(const Texture1DManager&) = delete;
  Texture1DManager& operator=(const Texture1DManager&) = delete;

  /// movable
  Texture1DManager(Texture1DManager&&) = default;
  Texture1DManager& operator=(Texture1DManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::size_t size, float* data, int channels = 4);

 private:
  using Base::active_instances_;
  using Base::free_instances_;
  int default_texture1d_id_ = 0; // adjust
};

} // namespace texture
} // namespace details


class Texture1D : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

  /// Base::Bind(GLenum) intentionally hided
  void Bind();

 private:
  using Base::opengl_id_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTURE1D_H_
