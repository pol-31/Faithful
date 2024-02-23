#ifndef FAITHFUL_SRC_COMMON_LOADINGMANAGER_H_
#define FAITHFUL_SRC_COMMON_LOADINGMANAGER_H_

#include "IGameManager.h"

#include "../executors/SupportThreadPool.h"

namespace faithful {
namespace details {

namespace assets {

class ModelPool;
class MusicPool;
class ShaderPool;
class SoundPool;
class Texture1DPool;
class Texture2DPool;

} // namespace assets

class LoadingManager : public IGameManager {
 public:
  LoadingManager() = delete;
  LoadingManager(
      assets::ModelPool* model_pool,
      assets::MusicPool* music_pool,
      assets::ShaderPool* shader_pool,
      assets::SoundPool* sound_pool,
      assets::Texture1DPool* texture1d_pool,
      assets::Texture2DPool* texture2d_pool);

  void Update() override;
  void Run() override;

  /// if too many tasks
  void StressLoading(int extra_threads_num = 4);

 private:
  assets::ModelPool* model_pool_;
  assets::MusicPool* music_pool_;
  assets::ShaderPool* shader_pool_;
  assets::SoundPool* sound_pool_;
  assets::Texture1DPool* texture1d_pool_;
  assets::Texture2DPool* texture2d_pool_;

  SupportThreadPool support_thread_pool_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_LOADINGMANAGER_H_
