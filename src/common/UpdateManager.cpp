#include "UpdateManager.h"

namespace faithful {
namespace details {

UpdateManager::UpdateManager(
    assets::ModelPool* model_manager, const Framerate& framerate)
    : model_manager_(model_manager),
      framerate_(framerate) {}

void UpdateManager::ProcessTask() {
  // TODO: all from task_queue_
}

void UpdateManager::UpdateAnimations() {
  // TODO: for each model call UpdateAnimation()
}
void UpdateManager::UpdateGameLogic() {
  // TODO: for each model call Update()
}

} // namespace details
} // namespace faithful
