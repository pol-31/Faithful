#include "LoadingModule.h"

#include <thread> // TODO: do we really need to create threads THERE?

namespace faithful {
namespace details {

LoadingModule::LoadingModule(
    assets::ModelPool& model_pool,
    assets::TexturePool& texture_pool)
    : model_pool_(model_pool),
      texture_pool_(texture_pool) {}

std::shared_ptr<assets::ModelData> LoadingModule::LoadModel(int model_id) {
  return model_pool_.Load(model_id);
}

void LoadingModule::ProcessTask() {
  if (!texture_pool_.Assist()) {
    if (!model_pool_.Assist()) {
      stress_loading_ = false;
    }
  }
}

void LoadingModule::StressLoading(int extra_threads_num) {
  if (stress_loading_) {
    return;
  }
  stress_loading_ = true;
  for (int i = 0; i < extra_threads_num; ++i) {
    std::thread{[this] {
      /// they both returns false if there is no task to execute
      if (!(texture_pool_.Assist() || model_pool_.Assist())) {
        return;
      }
    }}.detach();
  }
}

} // namespace details
} // namespace faithful
