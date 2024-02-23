#include "CollisionManager.h"

namespace faithful {
namespace details {

CollisionManager::CollisionManager(assets::ModelPool* model_manager)
    : model_manager_(model_manager) {}

void CollisionManager::Update() {
  // TODO: process bvh + all from task_queue_
}

void CollisionManager::Run() {
  //
}

} // namespace details
} // namespace faithful
