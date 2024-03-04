#include "CollisionManager.h"

namespace faithful {
namespace details {

CollisionManager::CollisionManager(
    assets::ModelPool* model_manager, const Framerate& framerate)
    : model_manager_(model_manager),
      framerate_(framerate) {}

void CollisionManager::ProcessTask() {
  // TODO: all from task_queue_
}

void CollisionManager::UpdateTree() {
  // TODO: update bvh tree
}

} // namespace details
} // namespace faithful
