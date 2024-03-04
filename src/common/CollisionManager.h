#ifndef FAITHFUL_COLLISIONMANAGER_H
#define FAITHFUL_COLLISIONMANAGER_H

#include "GlobalStateAwareBase.h"

#include "../collision/CollisionTarget.h" // TODO: do we need it

#include "FrameRate.h"

#include "../environment/Liquid.h"
#include "../environment/Terrain.h"
#include "../environment/Vegetation.h"

namespace faithful {

class PlayerCharacter;
class PhenomenonAreaPool;

namespace details {

namespace assets {

class ModelPool;

} // namespace assets

// StaticCollisionCheck -> assume we click1, click2, click3 ->
// -> queue_with_size_1 while processing click1, can't process anything else ->
// -> so if processing time of click1 is longer than time of pressing click2, ->
// -> then click2 skipped and we process only click3

/** This class should perform all collision checks (including accelerations)
 * and order then by their (possible) priority
 * */
class CollisionManager : public GlobalStateAwareBase {
 public:
  CollisionManager() = delete;
  CollisionManager(assets::ModelPool* model_manager,
                   const Framerate& framerate);

  void SetLiquidHandler(LiquidHandler* liquid_handler) {
    liquid_handler_ = liquid_handler;
  }
  void SetPhenomenonAreaPool(PhenomenonAreaPool* phenomenon_area_pool) {
    phenomenon_area_pool_ = phenomenon_area_pool;
  }
  void SetVegetationHandler(VegetationHandler* vegetation_handler) {
    vegetation_handler_ = vegetation_handler;
  }
  void SetPlayerCharacter(PlayerCharacter* player_character) {
    player_character_ = player_character;
  }
  void SetTerrain(Terrain* terrain) {
    terrain_ = terrain;
  }

  void ProcessTask();

  void UpdateTree();

  /// We have 3D game but with __camera_view__ from up to down which allows
  /// us to firstly check always 2D check and only then 3D check -> A-buffer CollDet

  inline void StateMenu() {
    menu_state_ = MenuState::kStartScreen;
    game_state_ = GameState::kPaused;
  }
  inline void StateGame() {
    game_state_ = GameState::kDefault;
    menu_state_ = MenuState::kPaused;
  }
  inline void StatePaused() {
    game_state_ = GameState::kPaused;
    menu_state_ = MenuState::kPaused;
  }

  inline void StateMenuLoadScreen() {
    menu_state_ = MenuState::kLoadScreen;
  }
  inline void StateMenuStartScreen() {
    menu_state_ = MenuState::kStartScreen;
  }
  inline void StateMenuConfigGeneral() {
    menu_state_ = MenuState::kConfigGeneral;
  }
  inline void StateMenuConfigLocalization() {
    menu_state_ = MenuState::kConfigLocalization;
  }
  inline void StateMenuConfigIO() {
    menu_state_ = MenuState::kConfigIO;
  }
  inline void StateMenuConfigSound() {
    menu_state_ = MenuState::kConfigSound;
  }
  inline void StateMenuConfigGraphic() {
    menu_state_ = MenuState::kConfigGraphic;
  }
  inline void StateMenuConfigKeys() {
    menu_state_ = MenuState::kConfigKeys;
  }

  inline void StateGameLoadScreen() {
    game_state_ = GameState::kLoadScreen;
  }
  inline void StateGameDefault() {
    game_state_ = GameState::kDefault;
  }
  inline void StateGameInventory1() {
    game_state_ = GameState::kInventory1;
  }
  inline void StateGameInventory2() {
    game_state_ = GameState::kInventory2;
  }
  inline void StateGameInventory3() {
    game_state_ = GameState::kInventory3;
  }
  inline void StateGameInventory4() {
    game_state_ = GameState::kInventory4;
  }
  inline void StateGameBattle() {
    game_state_ = GameState::kBattle;
  }
  inline void StateGameGamePause() {
    game_state_ = GameState::kGamePause;
  }

 private:
  enum class MenuState {
    kPaused,
    kLoadScreen,
    kStartScreen,
    kConfigGeneral,
    kConfigLocalization,
    kConfigIO,
    kConfigSound,
    kConfigGraphic,
    kConfigKeys
  };

  enum class GameState {
    kPaused,
    kLoadScreen,
    kDefault,
    kInventory1,
    kInventory2,
    kInventory3,
    kInventory4,
    kBattle,
    kGamePause
  };

  MenuState menu_state_;
  GameState game_state_;

  LiquidHandler* liquid_handler_ = nullptr; // get from model/player
  assets::ModelPool* model_manager_ = nullptr; // get from player/phenomenon_area
  PhenomenonAreaPool* phenomenon_area_pool_ = nullptr; // get from model/player
  VegetationHandler* vegetation_handler_ = nullptr; // get from model/player
  PlayerCharacter* player_character_ = nullptr; // get from model/phenomenon_area
  Terrain* terrain_ = nullptr;

  /** What collisions need to check:
   * 1) InstantDamage: emitted by model/player; affect all (can't affect emitter, so nice)
   * 2) phenomenon_area: emitted by model/player; temporary; affect all player/models (emitter included)
   * 3) liquid/vegetation at point P if player moving there
   * * */

  // actual collision checking:
  // for liquid / vegetation we just check current model/player position
  // for Attack / Input(player) we traverse last generated BVH tree
  /// if someone Move() then firstly should check direction from BVH
  /// TODO: but how to handle if multiple models are moving
  // for phenomenon area we just check current model/player position\

  //TODO:
  // Need to handle "tunneling" for:
  // ambient_types_, boss_types_, enemy_types_, npc_types_, player
  // -
  // solution: get framerate and of (e.g.) circle we check this:
  // (|||||)  <-sphere-swept rectangle
  // instead of:
  // ()       <-sphere

  // Example: cur_framerate = 60
  // for movable player we check ss-rectangle with length = player.speed * framerate
  // BUT better to use cur_framerate as an median to discard all noise



  void InitContext(); // TODO: allocate lhbvh, kdtree
  void DeinitContext(); // TODO: deallocate lhbvh, kdtree

  faithful::BVH* dynamic_data_ = nullptr;
  faithful::KDTree* static_data_ = nullptr;

  faithful::LHBVH_result dynamic_data_result;

  CollisionManager* collision_manager_ = nullptr;

  bool initialized_ = false;

  const Framerate& framerate_;
};

} // namespace details
} // namespace faithful

#endif // FAITHFUL_COLLISIONMANAGER_H
