#ifndef FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_
#define FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_

#include "IGameManager.h"

namespace faithful {

class PlayerCharacter;
class Sky;

namespace details {

namespace assets {

class ModelPool;

} // namespace assets
namespace environment {

class PhenomenonAreaPool;
class WeatherHandler;

} // namespace environment

/** This class should optimize the order of updating operations
 * to the OpenGL context to avoid consecutive Depth Test failures
 * */
class UpdateManager : public IGameManager {
 public:
  UpdateManager() = delete;
  UpdateManager(assets::ModelPool* model_manager);

  void SetPhenomenonAreaPool(environment::PhenomenonAreaPool* phenomenon_area_pool) {
    phenomenon_area_pool_ = phenomenon_area_pool;
  }
  void SetWeatherHandler(environment::WeatherHandler* weather_handler) {
    weather_handler_ = weather_handler;
  }
  void SetPlayerCharacter(PlayerCharacter* player_character) {
    player_character_ = player_character;
  }
  void SetSky(Sky* sky) {
    sky_ = sky;
  }

  void Update() override;
  void Run() override;

  inline void StatePaused() {
    scene_state_ = SceneState::kPaused;
  }
  inline void StateMenu() {
    scene_state_ = SceneState::kMainMenu;
  }
  inline void StateGame() {
    scene_state_ = SceneState::kMainGame;
  }

 private:
  enum class SceneState {
    kPaused,
    kMainMenu,
    kMainGame
  };

  SceneState scene_state_;

  // There are Update() based either on GlobalTime or some __state__
  // Vegetation, Liquid, Terrain doesn't depend on GlobalTime or __state__, so we don't need them there
  // Weather, Sky, PhenomenonAreaPool depend on GlobalTime

  /// __state__-based
  assets::ModelPool* model_manager_ = nullptr;
  PlayerCharacter* player_character_ = nullptr;

  /// Time-based
  environment::PhenomenonAreaPool* phenomenon_area_pool_ = nullptr;
  environment::WeatherHandler* weather_handler_ = nullptr;
  Sky* sky_ = nullptr;
};

} // namespace details
} // namespace faithful

#endif // FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_
