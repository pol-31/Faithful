#ifndef FAITHFUL_SRC_MODULES_COLLISIONMODULE_H_
#define FAITHFUL_SRC_MODULES_COLLISIONMODULE_H_

#include "../loader/ModelPool.h"

#include "../entities/PhenomenonArea.h"
#include "../environment/Environment.h"
#include "../player/PlayerCharacter.h"

#include "../collision/Bvh.h"

namespace faithful {
namespace details {

/** It perform all collision checks (including accelerations)
 * and order then by their (possible) priority
 * */
class CollisionModule {
 public:
  CollisionModule() = delete;
  CollisionModule(assets::ModelPool& model_pool,
                  Environment& environment,
                  PhenomenonAreaPool& phenomenon_area_pool,
                  PlayerCharacter& player_character);

  ~CollisionModule();

  void ProcessTask();

  void UpdateTree();

  /// We have 3D game but with __camera_view__ from up to down which allows
  /// us to firstly check always 2D check and only then 3D check -> A-buffer CollDet

 private:
  assets::ModelPool& model_pool_; // get from player/phenomenon_area
  LiquidHandler& liquid_handler_; // get from model/player
  PhenomenonAreaPool& phenomenon_area_pool_; // get from model/player
  PlayerCharacter& player_character_; // get from model/phenomenon_area
  TerrainHandler& terrain_handler_;
  VegetationHandler& vegetation_handler_; // get from model/player

  /** What collisions need to check:
   * 1) InstantDamage: emitted by model/player; affect all (can't affect emitter, so nice)
   * 2) phenomenon_area: emitted by model/player; temporary; affect all player/models (emitter included)
   * 3) liquid/vegetation at point P if player moving there
   * * */

  // actual collision checking:
  // for liquid / vegetation we just check current model/player position
  // for Attack / Input(player) we traverse last generated BVH tree
  /// if someone Move() then firstly should check direction from BVH
  // for phenomenon area we just check current model/player position\

  //TODO:
  // tunneling for movable targets:
  // get framerate and of (e.g.) circle we check this:
  // (|||||)  <-sphere-swept rectangle
  // instead of:
  // ()       <-sphere

  // Example: cur_framerate = 60
  // for movable player we check ss-rectangle with length = player.speed * framerate
  // BUT better to use cur_framerate as an median to discard all noise

  void Init();
  void DeInit();

  Bvh tree_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_COLLISIONMODULE_H_
