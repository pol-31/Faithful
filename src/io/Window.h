#ifndef FAITHFUL_WINDOW_H
#define FAITHFUL_WINDOW_H

#include <string_view>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <glm/glm.hpp>

#include "MonitorController.h"
#include "Monitor.h"
// #include "Common/Logger.h"

namespace faithful {
namespace details {
namespace io {

class Window {
 public:
  Window();
  ~Window();

  bool Init();

  // TODO: set title, icon

  void Deinit();

  void Resize();
  void Close();
  void FullscreenOn();
  void FullscreenOff();

  GLFWwindow* Glfw() {
    return window_;
  }

  MonitorController GetMonitorController() {
    return monitors_;
  }

  glm::vec2 MonitorResolution();

  void EnableVSync() { // TODO: should be enabled by default
    glfwSwapInterval(1);
  }

 private:
  glm::vec2 CalculateDefaultResolution();

  GLFWwindow* window_ = nullptr;
  Monitor cur_monitor_ = nullptr;
  MonitorController monitors_;
  glm::vec2 resolution_;
};

//TODO:
// forbid resizing
// glfwSetWindowCloseCallback(window, window_close_callback);
// glfwSetWindowContentScaleCallback(window, window_content_scale_callback);
void window_close_callback(GLFWwindow* window)
{
  if (!time_to_close)
    glfwSetWindowShouldClose(window, GLFW_FALSE);
}

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
  set_interface_scale(xscale, yscale);
}


void DefaultSizeCallback(GLFWwindow* window __attribute__((unused)), int width,
                         int height);

} // namespace io
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_WINDOW_H
