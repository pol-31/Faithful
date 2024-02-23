#include "DrawManager.h"

namespace faithful {
namespace details {

DrawManager::DrawManager(assets::ModelPool* model_manager)
    : model_manager_(model_manager) {}

void DrawManager::Update() {
  // TODO: process frame + all from task_queue_
}

void DrawManager::Run() {
  //
}

} // namespace details
} // namespace faithful
