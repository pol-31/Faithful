#ifndef FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_

#include "Executor.h"

#include <iostream> // todo: replace

#include "queues/LifoBoundedMPSCBlockingQueue.h"

#include "../Scene.h"

#include "../../config/Loader.h"

#include "../io/Window.h"

namespace faithful {
namespace details {

 /// Neither StaticThreadPool nor DynamicThreadPool
 /// This should be run from Main Thread and its blocking (after Run())

// Aggregate class (for scenes); switches between Scenes loops
// Camera/Cursor handles by current scene itself
class RenderThreadPool : public Executor {
 public:
  struct FrameRate {
    double last_; // TODO: is float enough?
    double delta_;
  };

  enum class GameState {
    kMainMenu,
    kMainGame,
    kTerminate
  };

  using Base = StaticExecutor<1>;

  RenderThreadPool();
  ~RenderThreadPool();

  void Run() override;
  void Join() override;

  void UpdateFramerate();
  void SetScene(GameState state) {
    game_state_ = state;
  }

 private:
  void ConfigMainMenu();
  void ConfigMainGame();

  void RunMainMenu();
  void RunMainGame();

  void ReleaseSources();

  void InitOpenGLContext();
  void DeinitOpenGLContext();

  /// Main Menu

  void ConfigLoadStartScreen() {

    UpdateFramerate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window_.Glfw());
    glfwPollEvents();
  }
  void DrawLoadStartScreen() {}
  void ProcessInputLoadStartScreen() {}

  void ConfigLoadPlayScreen() {}
  void DrawLoadPlayScreen() {}
  void ProcessInputLoadPlayScreen() {}

  void ConfigConfigGeneral() {}
  void DrawConfigGeneral() {}
  void ProcessInputConfigGeneral() {}

  void ConfigConfigLocalization() {}
  void DrawConfigLocalization() {}
  void ProcessInputConfigLocalization() {}

  void ConfigConfigIO() {}
  void DrawConfigIO() {}
  void ProcessInputConfigIO() {}

  void ConfigConfigSound() {}
  void DrawConfigSound() {}
  void ProcessInputConfigSound() {}

  void ConfigConfigGraphic() {}
  void DrawConfigGraphic() {}
  void ProcessInputConfigGraphic() {}

  void ConfigKeys() {}
  void DrawKeys() {}
  void ProcessInputKeys() {}

  /// Main Game

  void ConfigGameLoadScreen() {}
  void DrawGameLoadScreen() {}
  void ProcessInputGameLoadScreen() {}

  void ConfigGameDefault() {}
  void DrawGameDefault() {}
  void ProcessInputGameDefault() {}

  void ConfigInventory1() {}
  void DrawInventory1() {}
  void ProcessInputInventory1() {}

  void ConfigInventory2() {}
  void DrawInventory2() {}
  void ProcessInputInventory2() {}

  void ConfigInventory3() {}
  void DrawInventory3() {}
  void ProcessInputInventory3() {}

  void ConfigInventory4() {}
  void DrawInventory4() {}
  void ProcessInputInventory4() {}

  void ConfigGamePause() {}
  void DrawGamePause() {}
  void ProcessInputGamePause() {}

  io::Window window_;

  Camera camera_game_;

  GameState game_state_ = GameState::kMainMenu;

  faithful::details::io::Cursor cursor_arrow_;
  faithful::details::io::Cursor cursor_target_;


  faithful::details::io::Cursor* current_cursor_;


  MainMenuSceneState cur_menu_scene_ = MainMenuSceneState::kLoadStartScreen;
  MainGameSceneState cur_game_scene_ = MainGameSceneState::kGameDefault;

  FrameRate framerate_;
  folly::Function<bool()> stop_running_;

  bool opengl_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_
