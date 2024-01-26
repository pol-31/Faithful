#ifndef FAITHFUL_TEXTURE_H
#define FAITHFUL_TEXTURE_H

#include <array>
#include <filesystem>
#include <string>
#include <vector>

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
#include <astcenc.h>
#endif

#include <glad/glad.h>

#include "../utils/ConstexprVector.h"

#include "ResourceManager.h"

namespace faithful {

/* We need manager with info about how relevant OpenGL_img_id is, so
 * when need to load new texture then just reuse them.
 *
 * BUT we update relevancy only if we need new texture
 * AND how we do this:
 * assume we are at point (0;0), then with regard to:
 * - time
 * - position
 * - act (part of the game)
 * Then we get list of relevant textures for current position
 * Then iterating through active textures and if it in list - save, mark as useless otherwise
 *
 * How we know where we need new texture:
 * - time
 * - position (divide all map to regions --> like bucket sort)
 * (cache included)
 *
 * */

/* How loading occurs:
 * @params: OpenGL id = cur_id, int mode = 1d / 2d
 * 1) delete previous data from cur_id (if needed)
 * 2) ifdef FAITHFUL_OPENGL_SUPPORT_ASTC
 * 3) read file, parse header
 * 4) decompress (if astc not supported)
 * 5) load to cur_id
 * */


// TODO: implement global_id_ infrastructure <----------------------------------



inline constexpr int max_active_texture_num = 20;
inline constexpr int threads_per_texture = 4; // max / 2 || max -> +1 at static loading

/// should be only 1 instance for the entire program
template <int max_active_textures>
class TextureManager : public faithful::details::IResourceManager<max_active_textures> {
 public:
  using Base = faithful::details::IResourceManager<max_active_textures>;
  using InstanceInfo = typename Base::InstanceInfo;

  enum class TextureCategory {
    kLdr,
    kHdr,
    kNmap
  };

  TextureManager();
  ~TextureManager();

  /// not copyable
  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;

  /// movable
  TextureManager(TextureManager&&) = default;
  TextureManager& operator=(TextureManager&&) = default;

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

  void LoadTextureData(int active_instance_id);

  astcenc_context* PrepareContext(TextureCategory category);

  std::unique_ptr<uint8_t> DecompressAstcTexture(
      std::unique_ptr<uint8_t> tex_data, int tex_width,
      int tex_height, astcenc_context* context);

  TextureCategory DeduceTextureCategory(const std::filesystem::path& filename);
  bool DetectHdr(const std::filesystem::path& filename);
  bool DetectNmap(const std::filesystem::path& filename);

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  astcenc_context* context_ldr_ = nullptr;
  astcenc_context* context_hdr_ = nullptr;
  astcenc_context* context_nmap_ = nullptr;
#endif

  int default_texture_id_ = 0; // adjust
};


/// how to create textures from path: result from TextureManager.Load();
class Texture : public details::IResource {
 public:
  using Base = details::IResource;
  using Base::Base;
};

}  // namespace faithful

#endif  // FAITHFUL_TEXTURE_H
