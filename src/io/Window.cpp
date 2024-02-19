#include "Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <iostream>

#include "../../config/IO.h"

namespace faithful {
namespace details {
namespace io {

bool Window::Init() {
  cur_monitor_ = MonitorController::GetPrimaryMonitor();
  // TODO: handle monitor "init"


  /// don't need more hints, becaues we create only full-screen window
  /// and according to glfw.org needed hints set by default OR
  /// not affect full-screen window
  /// https://www.glfw.org/docs/3.3/window_guide.html
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glm::vec2 window_size = monitors_.CurrentMonitorResolution();

  GLFWwindow* window =
      glfwCreateWindow(window_size.x, window_size.y,
                       faithful::config::window_title,
                       cur_monitor_->Glfw(), nullptr);
  // simurgh::Logger::LogIf(simurgh::LogType::kFatal, !window)
  //   <<"Unable to create GLFW window";
  glfwMakeContextCurrent(window);
  window_ = window;

  AttachSizeCallback(DefaultSizeCallback);
  // TODO: callbacks
  return window;
}

Window::~Window() {
  glfwDestroyWindow(window_);
}

const glm::vec2& Window::Resolution() {
  return resolution_;
//  return cur_monitor_.GetResolution();
}

void Window::Resize() {
  GLint w, h;
  glfwGetWindowSize(window_, &w, &h);
  glViewport(0, 0, w, h);
}

void Window::Close() {
  glfwSetWindowShouldClose(window_, true);
}

void Window::FullscreenOn() {
  Monitor::Mode mode = monitors_.Current()->CurMode();
  glfwSetWindowMonitor(window_, monitors_.Current()->Glfw(), 0, 0, mode.width,
                       mode.height, mode.framerate);
}

void Window::FullscreenOff() {
  glfwSetWindowMonitor(window_, nullptr, 0, 0, resolution_.x, resolution_.y, 0);
}

void DefaultSizeCallback(GLFWwindow* window __attribute__((unused)), int width,
                         int height) {
  glViewport(0, 0, width, height);
}

} // namespace io
} // namespace details
} // namespace faithful
