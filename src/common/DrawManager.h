#ifndef FAITHFUL_SRC_COMMON_DRAWMANAGER_H_
#define FAITHFUL_SRC_COMMON_DRAWMANAGER_H_

#include "IGameManager.h"

namespace faithful {

class PlayerCharacter;
class Sky;
class Terrain;

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

/** This class should optimize the order of rendering operations
 * to the OpenGL context to avoid consecutive Depth Test failures
 * */
class DrawManager : public IGameManager {
 public:
  DrawManager() = delete;
  DrawManager(assets::ModelPool* model_manager);

  void SetLiquidHandler(environment::LiquidHandler* liquid_handler) {
    liquid_handler_ = liquid_handler;
  }
  void SetPhenomenonAreaPool(environment::PhenomenonAreaPool* phenomenon_area_pool) {
    phenomenon_area_pool_ = phenomenon_area_pool;
  }
  void SetVegetationHandler(environment::VegetationHandler* vegetation_handler) {
    vegetation_handler_ = vegetation_handler;
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
  void SetTerrain(Terrain* terrain) {
    terrain_ = terrain;
  }

  void Update() override;
  void Run() override;

  /// states

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

  environment::LiquidHandler* liquid_handler_ = nullptr;
  assets::ModelPool* model_manager_ = nullptr;
  environment::PhenomenonAreaPool* phenomenon_area_pool_ = nullptr;
  environment::VegetationHandler* vegetation_handler_ = nullptr;
  environment::WeatherHandler* weather_handler_ = nullptr;
  PlayerCharacter* player_character_ = nullptr;
  Sky* sky_ = nullptr;
  Terrain* terrain_ = nullptr;
};

} // namespace details
} // namespace faithful

#endif // FAITHFUL_SRC_COMMON_DRAWMANAGER_H_
