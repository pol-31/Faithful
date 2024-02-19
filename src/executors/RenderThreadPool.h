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

  using Base = StaticExecutor<1>;

  RenderThreadPool();
  ~RenderThreadPool();

  void Run() override;
  void Join() override;

  void UpdateFramerate();
  void SetScene(SceneType type) {
    cur_scene_ = type;
  }

 private:
  void RunMainMenuScene();
  void RunMainGameScene();

  void ReleaseSources();

  void InitOpenGLContext();
  void DeinitOpenGLContext();

  SceneType cur_scene_ = SceneType::kMainMenu;

  Scene main_menu_scene_;
  Scene main_game_scene_;

  FrameRate framerate_;
  folly::Function<bool()> stop_running_;

  io::Window window_;
  bool opengl_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_
