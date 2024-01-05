#ifndef FAITHFUL_WINDOW_H
#define FAITHFUL_WINDOW_H

#include <string_view>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <glm/glm.hpp>

#include "MonitorController.h"
// #include "Common/Logger.h"

namespace faithful {

namespace details {
namespace window {}  // namespace window
}  // namespace details

class Window {
 public:
  Window();
  Window(std::string_view title);
  Window(std::size_t width);
  Window(std::string_view title, std::size_t width);
  Window(std::size_t width, std::size_t height);
  Window(std::string_view title, std::size_t width, std::size_t height);

  // Window ctor:
  //   1) setting hints
  //   2) creating of GLFWwindow
  //   3) set default callbacks & attibutes

  // TODO: + all glfwWindow functions
  // TODO: + destructor (!!!)
  ~Window();

  void Resize();
  void Close();
  void FullscreenOn();
  void FullscreenOff();

  // conversion for convenient usage in GLFW-lib functions
  GLFWwindow* Glfw() {
    return window_;
  }

  void set_icon(int count, const GLFWimage* images) {
    glfwSetWindowIcon(window_, count, images);
  }

  void AttachPosCallback(void (*callback)(GLFWwindow* window, int xpos,
                                          int ypos)) {
    glfwSetWindowPosCallback(window_, callback);
  }
  void AttachSizeCallback(void (*callback)(GLFWwindow* window, int width,
                                           int height)) {
    glfwSetWindowSizeCallback(window_, callback);
  }
  void AttachCloseCallback(void (*callback)(GLFWwindow* window)) {
    glfwSetWindowCloseCallback(window_, callback);
  }
  void AttachRefreshCallback(void (*callback)(GLFWwindow* window)) {
    glfwSetWindowRefreshCallback(window_, callback);
  }
  void AttachFocusCallback(void (*callback)(GLFWwindow* window, int focused)) {
    glfwSetWindowFocusCallback(window_, callback);
  }
  void AttachIconifyCallback(void (*callback)(GLFWwindow* window,
                                              int iconified)) {
    glfwSetWindowIconifyCallback(window_, callback);
  }
  void AttachMaximizeCallback(void (*callback)(GLFWwindow* window,
                                               int maximized)) {
    glfwSetWindowMaximizeCallback(window_, callback);
  }
  void AttachContentScaleCallback(void (*callback)(GLFWwindow* window,
                                                   float xscale,
                                                   float yscale)) {
    glfwSetWindowContentScaleCallback(window_, callback);
  }

  GLFWwindow* GlfwWindow() {
    return window_;
  }

  MonitorController GetMonitorController() {
    return monitors_;
  }

  glm::vec2 MonitorResolution();

 private:
  glm::vec2 CalculateDefaultResolution();
  void CreateDefaultGlfwWindow(std::string_view title, std::size_t width,
                               std::size_t height);

  GLFWwindow* window_ = nullptr;
  MonitorController monitors_;
  glm::vec2 resolution_;
};

void DefaultSizeCallback(GLFWwindow* window __attribute__((unused)), int width,
                         int height);

}  // namespace faithful

#endif  // FAITHFUL_WINDOW_H
