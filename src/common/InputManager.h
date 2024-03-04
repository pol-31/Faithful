#ifndef FAITHFUL_SRC_COMMON_INPUTMANAGER_H_
#define FAITHFUL_SRC_COMMON_INPUTMANAGER_H_

#include "GlobalStateAwareBase.h"

namespace faithful {
namespace details {

class InputManager : public GlobalStateAwareBase {
 public:
  InputManager() = default;

  void Update() {
    // TODO: process pooling + set callbacks?
    //   task_queue_ not used?
  }

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

  inline void StateGameLoadScreen() {
    game_state_ = GameState::kLoadScreen;
  }
  inline void StateGameDefault() {
    game_state_ = GameState::kDefault;
  }
  inline void StateGameBattle() {
    game_state_ = GameState::kBattle;
  }
  inline void StateGameInventory() {
    game_state_ = GameState::kInventory;
  }

 private:
  enum class MenuState {
    kPaused,
    kLoadScreen,
    kStartScreen
  };
  enum class GameState {
    kPaused,
    kLoadScreen,
    kDefault,
    kBattle,
    kInventory
  };

  MenuState menu_state_;
  GameState game_state_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_INPUTMANAGER_H_
