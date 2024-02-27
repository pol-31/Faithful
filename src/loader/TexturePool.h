#ifndef FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_
#define FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_

#include <array>
#include <atomic>
#include <memory>
#include <string>
#include <queue>

#include <mutex>

//#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
#include <Source/astcenc.h>
//#endif

#include "assets_data/TextureData.h"

#include "IAssetPool.h"
#include "../../config/Loader.h"

#include "../../utils/Function.h"

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace assets {

/// should be only 1 instance for the entire program
class TexturePool
    : public IAssetPool<TextureData, faithful::config::texture_cache_size> {
 public:
  using Base = IAssetPool<TextureData, faithful::config::texture_cache_size>;
  using DataType = Base::DataType;

  TexturePool();
  ~TexturePool();

  /// not copyable
  TexturePool(const TexturePool&) = delete;
  TexturePool& operator=(const TexturePool&) = delete;

  /// not movable (because of mutexes)
  TexturePool(TexturePool&&) = delete;
  TexturePool& operator=(TexturePool&&) = delete;

  void SetOpenGlContext(DisplayInteractionThreadPool* opengl_context);

  void Assist();

 private:
  enum class TextureCategory {
    kLdr,
    kHdr,
    kNmap
  };

  struct AstcHeader {
    uint8_t magic[4];  /// format identifier
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    uint8_t dim_x[3];
    uint8_t dim_y[3];
    uint8_t dim_z[3];
  };

  // TODO: Is it blocking ? <<-- add thread-safety
  DataType LoadImpl(TrackedDataType& instance_info) override;


//#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  bool InitContextLdr();
  bool InitContextHdr();
  bool InitContextNmap();
//#endif

  void DecompressAstcTexture(TrackedDataType& instance_info,
                             std::unique_ptr<uint8_t[]> data,
                             int width, int height);

  astcenc_context* PrepareContext(TextureCategory category);

  TexturePool::TextureCategory DeduceTextureCategory(
      const std::string& filename);
  bool DetectHdr(const std::string& filename);
  bool DetectNmap(const std::string& filename);

  struct ProcessingContext {
    TexturePool::DataType data;
    folly::Function<void(int)> decoding_function;
    astcenc_context* context;
    int compressed_data_length;
    std::unique_ptr<uint8_t[]> compressed_data;
    astcenc_image image; // width, height, decompressed_data
    astcenc_swizzle swizzle;

    std::atomic_bool success = false;

    // 2 or 8 -- not atomic, because in Assist() it Guarded by mutex
    int free_thread_slots;
    std::atomic<int> working_threads_left;
  };

  std::mutex mutex_processing_tasks_;
  std::vector<ProcessingContext> processing_tasks_;

  DisplayInteractionThreadPool* opengl_context_ = nullptr;


//#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  astcenc_context* context_ldr_ = nullptr;
  astcenc_context* context_hdr_ = nullptr;
  astcenc_context* context_nmap_ = nullptr;
//#endif
};

} // namespace assets
} // namespace details
}  // namespace faithfu

#endif  // FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_
