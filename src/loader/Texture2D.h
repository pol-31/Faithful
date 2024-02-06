#ifndef FAITHFUL_TEXTURE_H
#define FAITHFUL_TEXTURE_H

#include <array>
#include <memory>
#include <string>

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
#include <astcenc.h>
#endif

#include "IAsset.h"
#include "../config/Loader.h"

namespace faithful {

// TODO: implement global_id_ infrastructure <----------------------------------

namespace details {
namespace texture {

/// should be only 1 instance for the entire program
class Texture2DManager : public faithful::details::IAssetManager<
                             faithful::config::max_active_texture2d_num> {
 public:
  using Base = faithful::details::IAssetManager<
      faithful::config::max_active_texture2d_num>;

  enum class TextureCategory {
    kLdr,
    kHdr,
    kNmap
  };

  Texture2DManager();
  ~Texture2DManager();

  /// not copyable
  Texture2DManager(const Texture2DManager&) = delete;
  Texture2DManager& operator=(const Texture2DManager&) = delete;

  /// movable
  Texture2DManager(Texture2DManager&&) = default;
  Texture2DManager& operator=(Texture2DManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& texture_path);

 private:
  struct AstcHeader {
    uint8_t magic[4];  /// format identifier
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    uint8_t dim_x[3];
    uint8_t dim_y[3];
    uint8_t dim_z[3];
  };

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  bool InitContextLdr();
  bool InitContextHdr();
  bool InitContextNmap();
#endif

  bool LoadTextureData(int active_instance_id);

  std::unique_ptr<uint8_t> DecompressAstcTexture(
      std::unique_ptr<uint8_t> tex_data, int tex_width,
      int tex_height, astcenc_context* context);

  astcenc_context* PrepareContext(Texture2DManager::TextureCategory category);

  Texture2DManager::TextureCategory DeduceTextureCategory(
      const std::string& filename);
  bool DetectHdr(const std::string& filename);
  bool DetectNmap(const std::string& filename);

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  astcenc_context* context_ldr_ = nullptr;
  astcenc_context* context_hdr_ = nullptr;
  astcenc_context* context_nmap_ = nullptr;
#endif
  using Base::active_instances_;
  using Base::free_instances_;
  int default_texture2d_id_ = 0; // adjust
};

} // namespace texture
} // namespace details


class Texture2D : public details::IAsset {
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

#endif  // FAITHFUL_TEXTURE_H
