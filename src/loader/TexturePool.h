#ifndef FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_
#define FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include <Source/astcenc.h>

#include "assets_data/TextureData.h"
#include "../common/OpenGlContextAwareBase.h"

#include "IAssetPool.h"
#include "../../config/Loader.h"
#include "../../utils/Function.h"

namespace faithful {
namespace details {
namespace assets {

class TexturePool
    : public IAssetPool<TextureData, faithful::config::kTextureCacheSize>,
      public OpenGlContextAwareBase {
 public:
  using Base = IAssetPool<TextureData, faithful::config::kTextureCacheSize>;
  using DataType = typename Base::DataType;

  TexturePool();
  ~TexturePool();

  /// not copyable
  TexturePool(const TexturePool&) = delete;
  TexturePool& operator=(const TexturePool&) = delete;

  /// not movable (because of mutex)
  TexturePool(TexturePool&&) = delete;
  TexturePool& operator=(TexturePool&&) = delete;

  /// returns true if there was executed a task,
  /// otherwise false, what means all have been already processed
  /// this is important for LoadingManager to understand phase of loading:
  /// normal or stress
  bool Assist();

 private:

  int max_thread_per_hdr_ = 2;
  int max_thread_per_ldr_ = 4;
  int max_thread_per_nmap_ = 4;

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

  /// thread-safe
  DataType LoadImpl(TrackedDataType& instance_info) override;

  bool InitContextLdr();
  bool InitContextHdr();
  bool InitContextNmap();

  astcenc_context* PrepareContext(TextureCategory category);

  TexturePool::TextureCategory DeduceTextureCategory(
      const std::string& filename);
  bool DetectHdr(const std::string& filename);
  bool DetectNmap(const std::string& filename);

  class ProcessingContext {
   public:
    TexturePool::DataType data;
    astcenc_context* context;
    TextureCategory category;
    int compressed_data_length;
    std::unique_ptr<uint8_t[]> compressed_data;
    astcenc_image image; // + width, height, decompressed_data
    astcenc_swizzle swizzle;

    /// why not atomic:
    /// - access to write is only to set false (nobody set true and
    ///     it doesn't matters in which order threads set false)
    /// - accessed to read only after all threads complete execution
    ///     (nobody can write to it)
    bool success = true;

    /// not atomic because accessed only by DecompressAstcTexture which
    /// is for one thread only OR under the mutex in Assist()
    int free_thread_slots;
    std::atomic<int> working_threads_left = 0;

    bool completed = false;

    void Decompress(int thread_id);

   private:
    void MakeComplete();
  };

  std::mutex mutex_processing_tasks_;
  std::vector<std::unique_ptr<ProcessingContext>> processing_tasks_;

  astcenc_context* context_ldr_ = nullptr;
  astcenc_context* context_hdr_ = nullptr;
  astcenc_context* context_nmap_ = nullptr;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTUREPOOL_H_
