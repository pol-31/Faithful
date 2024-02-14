#ifndef FAITHFUL_COLLISIONMANAGER_H
#define FAITHFUL_COLLISIONMANAGER_H

namespace faithful {

namespace details {
namespace assets {

class ModelPool;

} // namespace assets
namespace environment {

class LiquidHandler;
class PhenomenonAreaPool;
class VegetationHandler;

} // namespace environment
} // namespace details

class PlayerCharacter;
class Terrain;

/** This class should perform all collision checks (including accelerations)
 * and order then by their (possible) priority
 * */
class CollisionManager {
 public:
  CollisionManager() = default;

  void SetLiquidHandler(details::environment::LiquidHandler* liquid_handler) {
    liquid_handler_ = liquid_handler;
  }
  void SetModelPool(details::assets::ModelPool* model_manager) {
    model_manager_ = model_manager;
  }
  void SetPhenomenonAreaPool(details::environment::PhenomenonAreaPool* phenomenon_area_pool) {
    phenomenon_area_pool_ = phenomenon_area_pool;
  }
  void SetVegetationHandler(details::environment::VegetationHandler* vegetation_handler) {
    vegetation_handler_ = vegetation_handler;
  }
  void SetPlayerCharacter(PlayerCharacter* player_character) {
    player_character_ = player_character;
  }
  void SetTerrain(Terrain* terrain) {
    terrain_ = terrain;
  }

  /// We have 3D game but with __camera_view__ from up to down which allows
  /// us to firstly check always 2D check and only then 3D check -> A-buffer CollDet

 private:
  details::environment::LiquidHandler* liquid_handler_ = nullptr;
  details::assets::ModelPool* model_manager_ = nullptr;
  details::environment::PhenomenonAreaPool* phenomenon_area_pool_ = nullptr;
  details::environment::VegetationHandler* vegetation_handler_ = nullptr;
  PlayerCharacter* player_character_ = nullptr;
  Terrain* terrain_ = nullptr;
};

}  // namespace faithful

#endif  // FAITHFUL_COLLISIONMANAGER_H
