#ifndef FAITHFUL_SRC_COMMON_LOADINGMANAGER_H_
#define FAITHFUL_SRC_COMMON_LOADINGMANAGER_H_

#include <memory>

#include "../loader/assets_data/ModelData.h"
#include "GlobalStateAwareBase.h"

namespace faithful {
namespace details {

namespace assets {

class ModelPool;
class TexturePool;

} // namespace assets

class LoadingManager : public GlobalStateAwareBase {
 public:
  LoadingManager() = delete;
  LoadingManager(
      assets::ModelPool* model_pool,
      assets::TexturePool* texture_pool);

  /// textures / sounds loaded by models;
  /// hdr sky texture and music handled not there,
  /// because music is streamed by AudioThreadPool, sky loaded only
  /// once at the beginning of the program;
  /// shaders loaded only once at the beginning of the program
  /// TOTAL: sky / shaders once, not there; audio by AudioThreadPool;
  /// models and related sounds/textures there
  std::shared_ptr<assets::ModelData> LoadModel(int model_id);

  /// takes one (only 1) task from the queue and execute it
  void ProcessTask();

  /// if too many tasks; thread-unsafe
  void StressLoading(int extra_threads_num = 4);

 private:

  assets::ModelPool* model_pool_;
  assets::TexturePool* texture_pool_;

  bool stress_loading_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_LOADINGMANAGER_H_
