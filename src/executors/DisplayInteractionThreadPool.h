#ifndef FAITHFUL_SRC_EXECUTORS_DISPLAYINTERACTIONTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_DISPLAYINTERACTIONTHREADPOOL_H_

#include "Executor.h"

#include <iostream> // todo: replace

#include "../io/Window.h"
#include "../io/Camera.h"
#include "../io/Cursor.h"

#include "../../assets/embedded/CursorMainMenu.h"
#include "../../assets/embedded/CursorMainGame.h"

#include "../common/FrameRate.h"

namespace faithful {
namespace details {

class DrawManager;

void GlfwErrorCallback(int error, const char *description);
void GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height);

/// Stands for both Input handling(ProcessInput()) and rendering (Drawing()),
/// which violated Single-Responsibility principle, but currently it's not
/// possible to separate these entities due to GLFW and Glad requirements
/// where both should be run from the same thread (main render function,
/// calls both to OpenGL and GLFW functions (e.g. glfwPollEvents(), glClear()))
/// Neither StaticThreadPool nor DynamicThreadPool
/// This should be run from Main Thread and its blocking (after Run())
/// "1" - because we need only ONE GLFW & Glad context
class DisplayInteractionThreadPool : public Executor {
 public:
  DisplayInteractionThreadPool() = delete;
  DisplayInteractionThreadPool(DrawManager* music_manager);

  ~DisplayInteractionThreadPool();

  void Run() override;
  void Join() override;

 private:
  void InitContext();
  void DeinitContext();

  void RunMainGame();
  void RunMainMenu();

  void processInput(GLFWwindow *window);

  io::Window window_;

  Camera camera_game_;

  faithful::details::io::Cursor cursor_arrow_;
  faithful::details::io::Cursor cursor_target_;

  faithful::details::io::Cursor* current_cursor_;

  Framerate framerate_;
  bool need_to_update_monitor_ = false;

  bool opengl_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_DISPLAYINTERACTIONTHREADPOOL_H_
