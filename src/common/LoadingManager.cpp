#include "LoadingManager.h"

namespace faithful {
namespace details {

LoadingManager::LoadingManager(
    assets::ModelPool* model_pool,
    assets::MusicPool* music_pool,
    assets::ShaderPool* shader_pool,
    assets::SoundPool* sound_pool,
    assets::Texture1DPool* texture1d_pool,
    assets::Texture2DPool* texture2d_pool)
    : model_pool_(model_pool), music_pool_(music_pool),
      shader_pool_(shader_pool), sound_pool_(sound_pool),
      texture1d_pool_(texture1d_pool), texture2d_pool_(texture2d_pool),
      support_thread_pool_(task_queue_) {}

void LoadingManager::Update() {
  // TODO: process all loading + all from task_queue_
}
void LoadingManager::Run() {
  //
}

void LoadingManager::StressLoading(int extra_threads_num) {
  //TODO:
  // occupancies
  // stress loading: we take half of all tasks there at once
  // supporting thread pool
  // report back to FaithfulState
  // NOTIFY GameLogicThreadPool
}

} // namespace details
} // namespace faithful
