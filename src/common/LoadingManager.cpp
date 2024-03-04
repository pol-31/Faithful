#include "LoadingManager.h"

#include <thread>

#include "../loader/ModelPool.h"
#include "../loader/TexturePool.h"

namespace faithful {
namespace details {

LoadingManager::LoadingManager(
    assets::ModelPool* model_pool,
    assets::TexturePool* texture_pool)
    : model_pool_(model_pool),
      texture_pool_(texture_pool) {}

std::shared_ptr<assets::ModelData> LoadingManager::LoadModel(int model_id) {
  return model_pool_->Load(model_id);
}

void LoadingManager::ProcessTask() {
  if (!texture_pool_->Assist()) {
    if (!model_pool_->Assist()) {
      stress_loading_ = false;
    }
  }
}

void LoadingManager::StressLoading(int extra_threads_num) {
  if (stress_loading_) {
    return;
  }
  stress_loading_ = true;
  for (int i = 0; i < extra_threads_num; ++i) {
    std::thread{[=] {
      /// they both returns false if there is no task to execute
        if (!(texture_pool_->Assist() || model_pool_->Assist())) {
          return;
        }
    }}.detach();
  }
}

} // namespace details
} // namespace faithful
