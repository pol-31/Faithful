#include "ModelPool.h"

#include <glm/gtc/quaternion.hpp>

namespace faithful {
namespace details {
namespace assets {

ModelPool::ModelPool() {
  //  for (int i = 0; i < faithful::config::max_active_sound_num; ++i) {
  //    active_instances_[i].internal_id = i;
  //  }
    // TODO: vao, vbo
}

ModelPool::~ModelPool() {
  // TODO: release all vao, vbo
}

AssetInstanceInfo ModelPool::Load(std::string&& model_path,
                                  faithful::EntityType type_tag) {
  switch (type_tag) {
    case EntityType::kAmbient:
      ambient_instances_.Add(ambient_types_.LoadAmbient(std::move(model_path)));
      break;
    case EntityType::kBoss:
      boss_instances_.Add(boss_types_.LoadBoss(std::move(model_path)));
      break;
    case EntityType::kEnemy:
      enemy_instances_.Add(enemy_types_.LoadEnemy(std::move(model_path)));
      break;
    case EntityType::kFurniture:
      furniture_instances_.Add(furniture_types_.LoadFurniture(std::move(model_path)));
      break;
    case EntityType::kLoot:
      loot_instances_.Add(loot_types_.LoadLoot(std::move(model_path)));
      break;
    case EntityType::kNPC:
      npc_instances_.Add(npc_types_.LoadNPC(std::move(model_path)));
      break;
    case EntityType::kObstacle:
      obstacle_instances_.Add(obstacle_types_.LoadObstacle(std::move(model_path)));
      break;
  }
}

} // namespace assets
} // namespace details
} // namespace faithful