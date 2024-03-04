#include "Window.h"

#include <iostream> // Logger.h

#include "../../config/IO.h" // for window title
#include "../../assets/embedded/WindowIcon.h"

namespace faithful {
namespace details {
namespace io {

Window::Window() {
  Init();
}

Window::~Window() {
  DeInit();
}

void Window::Init() {
  /// don't need more hints, (one full-screen window there)
  /// https://www.glfw.org/docs/3.3/window_guide.html
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  resolution_ = CalculateDefaultResolution();

  GLFWwindow* window = glfwCreateWindow(
      resolution_.x, resolution_.y,
      faithful::config::kWindowTitle,
      monitors_info_.GetCurMonitor().Glfw(),
      nullptr); // no screen resources sharing
  if (!window) {
    std::cerr << "Window::Init() can't create window" << std::endl;
    std::terminate(); // TODO: FAITHFUL_UNREACHABLE
  }

  GLFWimage icon;
  icon.width = faithful::embedded::kWindowIconWidth;
  icon.height = faithful::embedded::kWindowIconHeight;
  icon.pixels = const_cast<unsigned char*>(faithful::embedded::kWindowIconData);
  glfwSetWindowIcon(glfw_window_, 1, &icon);

  monitors_info_.SetWindowMonitorMode(
      glfw_window_, monitors_info_.GetCurMonitor().GetCurModeId());

  glfwMakeContextCurrent(window);

  EnableVSync();
}

void Window::DeInit() {
  glfwDestroyWindow(glfw_window_);
}

void Window::Update() {
  monitors_info_.Update(glfwGetWindowMonitor(glfw_window_));
  resolution_ = monitors_info_.GetCurMonitor().GetCurResolution();
}

void Window::EnableVSync() {
  glfwSwapInterval(1);
  enable_vsync_ = true;
}

void Window::DisableVSync() {
  glfwSwapInterval(0);
  enable_vsync_ = false;
}

glm::ivec2 Window::CalculateDefaultResolution() {
  return monitors_info_.GetCurMonitor().GetCurResolution();
}

} // namespace io
} // namespace details
} // namespace faithful
