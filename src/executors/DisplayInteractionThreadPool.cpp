#include "DisplayInteractionThreadPool.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../common/DrawManager.h"
#include "../common/InputManager.h"

#include "ExecutionEnvironment.h"

namespace faithful {
namespace details {

void GlfwWindowCloseCallback(GLFWwindow* window) {
  void* data = glfwGetWindowUserPointer(window);
  reinterpret_cast<GlfwWindowUserPointer*>(data)
      ->execution_environment->Join();
}

void GlfwErrorCallback(int error __attribute__((unused)),
                       const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void GlfwFramebufferSizeCallback(GLFWwindow* window __attribute__((unused)),
                                 int width, int height) {
  glViewport(0, 0, width, height);
}

DisplayInteractionThreadPool::GlfwInitializer::GlfwInitializer() {
  if (!glfwInit()) {
    std::cerr << "glfw init error" << std::endl;
    std::terminate();
  }
  glfwSetErrorCallback(GlfwErrorCallback);
  // TODO: set OpenGL & GLFW error callbacks depends on FAITHFUL_DEBUG / ext
}
DisplayInteractionThreadPool::GlfwInitializer::~GlfwInitializer() {
  glfwTerminate();
}

void DisplayInteractionThreadPool::SetGlfwWindowUserPointer(void* data) {
  glfwSetWindowUserPointer(window_.Glfw(), data);
}
void DisplayInteractionThreadPool::UnSetGlfwWindowUserPointer() {
  glfwSetWindowUserPointer(window_.Glfw(), nullptr);
}

/// such weird constructor because Cursor and Camera classes have
/// explicitly deleted ctor (there's no sense to use it) and
/// should be initialized with class faithful::Window
DisplayInteractionThreadPool::DisplayInteractionThreadPool(
    DrawManager* draw_manager, InputManager* input_manager)
    : glfw_initializer_(), // glfw initialized here
      window_(),
      camera_game_(window_.GetResolution()),
      cursor_arrow_(faithful::embedded::cursor_main_menu_data,
                    faithful::embedded::cursor_main_menu_width,
                    faithful::embedded::cursor_main_menu_height),
      cursor_target_(faithful::embedded::cursor_main_game_data,
                     faithful::embedded::cursor_main_game_width,
                     faithful::embedded::cursor_main_game_height),
      current_cursor_(&cursor_arrow_),
      draw_manager_(draw_manager),
      input_manager_(input_manager) {
  Init();
}

DisplayInteractionThreadPool::~DisplayInteractionThreadPool() {
  if (state_ == IExecutor::State::kRunning) {
    Join();
  }
}

void DisplayInteractionThreadPool::Join() {
  state_ = IExecutor::State::kJoined;
  processing_state_ = ProcessingState::kJoined;
  while (state_ != IExecutor::State::kJoined) {}
}

void DisplayInteractionThreadPool::Init() {
  /// set GlfwMonitorUserPointer for getting info about monitor connect/disconnect
  window_.GetMonitorInfoRef()
      .InitUpdateRef(reinterpret_cast<void*>(&need_to_update_monitor_));
  glfwSetWindowCloseCallback(window_.Glfw(), GlfwWindowCloseCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    std::terminate();
  }
  glfwSetFramebufferSizeCallback(window_.Glfw(), GlfwFramebufferSizeCallback);

  glClearColor(0.2, 0.2, 0.2, 1.0);
}

void DisplayInteractionThreadPool::Run() {
  while (state_ != State::kJoined) {
    while (processing_state_ == ProcessingState::kMenu) {
      draw_manager_->Update();
      input_manager_->Update();
    }
    while (processing_state_ == ProcessingState::kGame) {
      draw_manager_->Update();
      input_manager_->Update();
    }
    if (processing_state_ == ProcessingState::kJoined) {
      break;
    }
  }
  state_ = IExecutor::State::kJoined;
}

} // namespace details
} // namespace faithful