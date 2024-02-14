#ifndef FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_
#define FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_

namespace faithful {

namespace details {
namespace assets {

class ModelPool;

} // namespace assets
namespace environment {

class PhenomenonAreaPool;
class WeatherHandler;

} // namespace environment
} // namespace details

class PlayerCharacter;
class Sky;

/** This class should optimize the order of updating operations
 * to the OpenGL context to avoid consecutive Depth Test failures
 * */
class UpdateManager {
 public:
  UpdateManager() = default;

  void SetModelPool(details::assets::ModelPool* model_manager) {
    model_manager_ = model_manager;
  }
  void SetPhenomenonAreaPool(details::environment::PhenomenonAreaPool* phenomenon_area_pool) {
    phenomenon_area_pool_ = phenomenon_area_pool;
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

 private:
  // There are Update() based either on GlobalTime or some __state__
  // Vegetation, Liquid, Terrain doesn't depend on GlobalTime or __state__, so we don't need them there
  // Weather, Sky, PhenomenonAreaPool depend on GlobalTime

  /// __state__-based
  details::assets::ModelPool* model_manager_ = nullptr;
  PlayerCharacter* player_character_ = nullptr;

  /// Time-based
  details::environment::PhenomenonAreaPool* phenomenon_area_pool_ = nullptr;
  details::environment::WeatherHandler* weather_handler_ = nullptr;
  Sky* sky_ = nullptr;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_
