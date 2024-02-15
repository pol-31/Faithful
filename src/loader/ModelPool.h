#ifndef FAITHFUL_SRC_LOADER_MODELPOOL_H_
#define FAITHFUL_SRC_LOADER_MODELPOOL_H_

#include <string>

#include "AssetInstanceInfo.h"

#include "../entities/EntityTraits.h"
#include "../config/Entities.h"

#include "model/CreatureModelPools.h"
#include "model/FurnitureModelPools.h"
#include "model/ImmovableModelPools.h"

namespace faithful {
namespace details {
namespace assets {

/// Wrapper for all model types, serves as a connection point between
/// user interface & implementation
/// Handles all vao, vbo
/// should be only 1 instance for the entire program

// TODO: vao, vbo, ubo, etc...
//  unsigned int vao_ = 0;
//  unsigned int vbo_ = 0;
//  unsigned int ebo_ = 0;
//  unsigned int ebo_idx_num_ = 0;

class ModelPool {
 public:
  ModelPool();
  ~ModelPool();

  /// not copyable
  ModelPool(const ModelPool&) = delete;
  ModelPool& operator=(const ModelPool&) = delete;

  /// movable
  ModelPool(ModelPool&&) = default;
  ModelPool& operator=(ModelPool&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  AssetInstanceInfo Load(std::string&& model_path, EntityType type_tag);

 private:
  /// We've separated managing of types & instances, because
  /// they are absolutely different things:
  /// type: responsible for loading/uploading from memory, stores
  ///       general for model info like vertex data, normals, materials
  /// instance: responsible for distinct ubo/shader data, because
  ///           at one point of time different instances might have different poses

  /// unique types
  CreatureModelTypePool<config::max_types::kAmbient> ambient_types_;
  CreatureModelTypePool<config::max_types::kBoss> boss_types_;
  CreatureModelTypePool<config::max_types::kEnemy> enemy_types_;
  FurnitureModelTypePool<config::max_types::kFurniture> furniture_types_;
  ImmovableModelTypePool<config::max_types::kLoot> loot_types_;
  CreatureModelTypePool<config::max_types::kNPC> npc_types_;
  ImmovableModelTypePool<config::max_types::kObstacle> obstacle_types_;

  /// unique instances
  CreatureModelInstancePool<config::max_instances::kAmbient> ambient_instances_;
  CreatureModelInstancePool<config::max_instances::kBoss> boss_instances_;
  CreatureModelInstancePool<config::max_instances::kEnemy> enemy_instances_;
  FurnitureModelInstancePool<config::max_instances::kFurniture> furniture_instances_;
  ImmovableModelInstancePool<config::max_instances::kLoot> loot_instances_;
  CreatureModelInstancePool<config::max_instances::kNPC> npc_instances_;
  ImmovableModelInstancePool<config::max_instances::kObstacle> obstacle_instances_;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODELPOOL_H_
