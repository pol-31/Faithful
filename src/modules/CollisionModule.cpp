#include "CollisionModule.h"

namespace faithful {
namespace details {

CollisionModule::CollisionModule(
    assets::ModelPool& model_pool,
    Environment& environment,
    PhenomenonAreaPool& phenomenon_area_pool,
    PlayerCharacter& player_character)
    : model_pool_(model_pool),
      liquid_handler_(environment.liquid_handler),
      phenomenon_area_pool_(phenomenon_area_pool),
      player_character_(player_character),
      terrain_handler_(environment.terrain_handler),
      vegetation_handler_(environment.vegetation_handler) {
  Init();
}

CollisionModule::~CollisionModule() {
  DeInit();
}

void CollisionModule::ProcessTask() {
  // TODO: all from task_queue_
}

void CollisionModule::UpdateTree() {
  // TODO: update bvh tree
}

void CollisionModule::Init() {
  // TODO: allocate all data for bvh
}

void CollisionModule::DeInit() {
  // TODO: deallocate all data for bvh
}

} // namespace details
} // namespace faithful
