#include "Monitor.h"

#include "Window.h"

namespace faithful {
namespace details {
namespace io {

Monitor::Monitor(GLFWmonitor* glfw_monitor) {
  glfw_monitor_ = glfw_monitor;
  name_ = glfwGetMonitorName(glfw_monitor_);
  int modes_num;
  const GLFWvidmode* modes = glfwGetVideoModes(glfw_monitor, &modes_num);
  modes_ = ModesType{static_cast<ModesType::SizeType>(modes_num), modes};
  const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitor_); // cur_mode chosen by GLFW
  for (int i = 0; i < modes_num; ++i) {
    if (modes_[i].width == mode->width &&
        modes_[i].height == mode->height &&
        modes_[i].refreshRate == mode->refreshRate) {
      cur_mode_id_ = i;
    }
  }
}

void Monitor::SetMonitor(Window* window) {
  const GLFWvidmode& new_mode = modes_[cur_mode_id_];
  glfwSetWindowMonitor(window->Glfw(), glfw_monitor_, 0, 0, new_mode.width,
                       new_mode.height, new_mode.refreshRate);
}

void Monitor::SetMode(Window* window, int mode_id) {
  const GLFWvidmode& new_mode = modes_[mode_id];
  cur_mode_id_ = mode_id;
  glfwSetWindowMonitor(window->Glfw(), glfw_monitor_, 0, 0, new_mode.width,
                       new_mode.height, new_mode.refreshRate);
}

}  // namespace io
}  // namespace details
}  // namespace faithful