#ifndef FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_
#define FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_

#include "GlobalStateAwareBase.h"

#include "FrameRate.h"

#include "../environment/Sky.h"
#include "../environment/Weather.h"

namespace faithful {

class PlayerCharacter;
class PhenomenonAreaPool;

namespace details {

namespace assets {

class ModelPool;

} // namespace assets

/** This class should optimize the order of updating operations
 * to the OpenGL context to avoid consecutive Depth Test failures
 * */
class UpdateManager : public GlobalStateAwareBase {
 public:
  UpdateManager() = delete;
  UpdateManager(assets::ModelPool* model_manager,
                const Framerate& framerate);

  void SetPhenomenonAreaPool(PhenomenonAreaPool* phenomenon_area_pool) {
    phenomenon_area_pool_ = phenomenon_area_pool;
  }
  void SetWeatherHandler(WeatherHandler* weather_handler) {
    weather_handler_ = weather_handler;
  }
  void SetPlayerCharacter(PlayerCharacter* player_character) {
    player_character_ = player_character;
  }
  void SetSky(Sky* sky) {
    sky_ = sky;
  }

  void ProcessTask();

  // updates all models actions logic / affects;
  // phenomenon area (e.g. gas intensity)
  void UpdateGameLogic();
  void UpdateAnimations();

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
  enum class SceneState { // TODO do we still need this?..
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
  PhenomenonAreaPool* phenomenon_area_pool_ = nullptr;
  WeatherHandler* weather_handler_ = nullptr;
  Sky* sky_ = nullptr;

  const Framerate& framerate_;
};

} // namespace details
} // namespace faithful

#endif // FAITHFUL_SRC_COMMON_UPDATEMANAGER_H_
