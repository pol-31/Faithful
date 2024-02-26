#ifndef FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_
#define FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_

#include <array>
#include <memory>
#include <string>

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
#include <Source/astcenc.h>
#endif

#include "IAssetPool.h"
#include "AssetInstanceInfo.h"
#include "../../config/Loader.h"

namespace faithful {
namespace details {
namespace assets {

/// should be only 1 instance for the entire program
class TexturePool
    : public IAssetPool<
                          faithful::config::max_active_texture2d_num> {
 public:
  using Base = IAssetPool<faithful::config::max_active_texture2d_num>;

  enum class TextureCategory {
    kLdr,
    kHdr,
    kNmap
  };

  TexturePool();
  ~TexturePool();

  /// not copyable
  TexturePool(const TexturePool&) = delete;
  TexturePool& operator=(const TexturePool&) = delete;

  /// movable
  TexturePool(TexturePool&&) = default;
  TexturePool& operator=(TexturePool&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  AssetInstanceInfo Load(std::string&& texture_path);

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

  astcenc_context* PrepareContext(TexturePool::TextureCategory category);

  TexturePool::TextureCategory DeduceTextureCategory(
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
  using Base::default_id_; // TODO: use it inside the Load()
};

} // namespace assets
} // namespace details
}  // namespace faithfu

#endif  // FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_
