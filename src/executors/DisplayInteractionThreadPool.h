#ifndef FAITHFUL_SRC_EXECUTORS_DISPLAYINTERACTIONTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_DISPLAYINTERACTIONTHREADPOOL_H_

#include "IExecutor.h"

#include <iostream> // todo: replace

#include "../io/Window.h"
#include "../io/Camera.h"
#include "../io/Cursor.h"

#include "../../assets/embedded/CursorMainMenu.h"
#include "../../assets/embedded/CursorMainGame.h"

#include "../common/GlobalStateInfo.h"

#include "../common/FrameRate.h"

namespace faithful {
namespace details {

class DrawManager;
class InputManager;

void GlfwWindowCloseCallback(GLFWwindow* window);
void GlfwErrorCallback(int error, const char *description);
void GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height);

/// Stands for both Input handling(ProcessInput()) and rendering (Drawing()),
/// which violated Single-Responsibility principle, but currently it's not
/// possible to separate these entities due to GLFW and Glad requirements
/// where both should be run from the same thread (main render function,
/// calls both to OpenGL and GLFW functions (e.g. glfwPollEvents(), glClear()))
/// Neither StaticThreadPool nor DynamicThreadPool
/// This should be run from Main Thread and its blocking (after Run())
class DisplayInteractionThreadPool : public IExecutor {
 public:
  DisplayInteractionThreadPool() = delete;
  DisplayInteractionThreadPool(DrawManager* draw_manager,
                               InputManager* input_manager);

  ~DisplayInteractionThreadPool();

  void Run() override;
  void Join() override;

 protected:
  friend class ExecutionEnvironment;
  void SetGlfwWindowUserPointer(void* data);
  void UnSetGlfwWindowUserPointer();

 private:
  enum class ProcessingState {
    kMenu,
    kGame,
    kJoined
  };

  ProcessingState processing_state_;

  // TODO: explain this 1000iq high-brain supremacy domination move
  //  (because Window, Cursor, Camera
  //  don't have default ctor and all should be aware of window and
  //  also glfwInit() (because window can't be created without glfwInit()))
  //  -- and the same "issue" with ExecutionEnvironment() default ctor
  struct GlfwInitializer {
    GlfwInitializer();
    ~GlfwInitializer(); // glfw deinit at class dtor
  };

  void Init();

  GlfwInitializer glfw_initializer_;

  io::Window window_;

  Camera camera_game_;

  faithful::details::io::Cursor cursor_arrow_;
  faithful::details::io::Cursor cursor_target_;

  faithful::details::io::Cursor* current_cursor_;

  DrawManager* draw_manager_;
  InputManager* input_manager_;

  /// We don't use our own task_queue_
  // NOT using Base::task_queue_;

  Framerate framerate_; // TODO: integrate
  bool need_to_update_monitor_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_DISPLAYINTERACTIONTHREADPOOL_H_
