#include "UpdateManager.h"

namespace faithful {
namespace details {

UpdateManager::UpdateManager(assets::ModelPool* model_manager)
    : model_manager_(model_manager) {}

void UpdateManager::Update() {
  // TODO: process all Updatable instances + all from task_queue_
}

void UpdateManager::Run() {
  //
}

} // namespace details
} // namespace faithful
