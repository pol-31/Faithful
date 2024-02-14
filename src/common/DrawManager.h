#ifndef FAITHFUL_SRC_COMMON_DRAWMANAGER_H_
#define FAITHFUL_SRC_COMMON_DRAWMANAGER_H_

namespace faithful {

namespace details {
namespace assets {

class ModelPool;

} // namespace assets
namespace environment {

class LiquidHandler;
class PhenomenonAreaPool;
class VegetationHandler;
class WeatherHandler;

} // namespace environment
} // namespace details

class PlayerCharacter;
class Sky;
class Terrain;

/** This class should optimize the order of rendering operations
 * to the OpenGL context to avoid consecutive Depth Test failures
 * */
class DrawManager {
 public:
  DrawManager() = default;

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
  void SetWeatherHandler(details::environment::WeatherHandler* weather_handler) {
    weather_handler_ = weather_handler;
  }
  void SetPlayerCharacter(PlayerCharacter* player_character) {
    player_character_ = player_character;
  }
  void SetSky(Sky* sky) {
    sky_ = sky;
  }
  void SetTerrain(Terrain* terrain) {
    terrain_ = terrain;
  }

 private:
  details::environment::LiquidHandler* liquid_handler_ = nullptr;
  details::assets::ModelPool* model_manager_ = nullptr;
  details::environment::PhenomenonAreaPool* phenomenon_area_pool_ = nullptr;
  details::environment::VegetationHandler* vegetation_handler_ = nullptr;
  details::environment::WeatherHandler* weather_handler_ = nullptr;
  PlayerCharacter* player_character_ = nullptr;
  Sky* sky_ = nullptr;
  Terrain* terrain_ = nullptr;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_DRAWMANAGER_H_
