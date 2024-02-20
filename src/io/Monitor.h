#ifndef FAITHFUL_MONITOR_H
#define FAITHFUL_MONITOR_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../utils/Span.h"

namespace faithful {
namespace details {
namespace io {

class Monitor {
 public:
  using ModesType = faithful::utils::Span<const GLFWvidmode>;

  Monitor() : Monitor(glfwGetPrimaryMonitor()) {}

  Monitor(const Monitor&) = default;
  Monitor& operator=(const Monitor&) = default;

  Monitor(Monitor&&) = default;
  Monitor& operator=(Monitor&&) = default;

  Monitor(GLFWmonitor* glfw_monitor) {
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

  GLFWmonitor* Glfw() {
    return glfw_monitor_;
  }

  const ModesType& GetModes() const {
    return modes_;
  }
  int GetCurModeId() const {
    return cur_mode_id_;
  }

  glm::ivec2 GetCurResolution() {
    return {modes_[cur_mode_id_].width, modes_[cur_mode_id_].height};
  }
  int GetCurFramerate() {
    return modes_[cur_mode_id_].refreshRate;
  }

  void SetMonitor(GLFWwindow* glfw_window) {
    const GLFWvidmode& new_mode = modes_[cur_mode_id_];
    glfwSetWindowMonitor(glfw_window, glfw_monitor_, 0, 0, new_mode.width,
                         new_mode.height, new_mode.refreshRate);
  }

  // also sets monitor
  void SetMode(GLFWwindow* glfw_window, int mode_id) {
    const GLFWvidmode& new_mode = modes_[mode_id];
    cur_mode_id_ = mode_id;
    glfwSetWindowMonitor(glfw_window, glfw_monitor_, 0, 0, new_mode.width,
                         new_mode.height, new_mode.refreshRate);
  }

 private:
  ModesType modes_;
  GLFWmonitor* glfw_monitor_;
  const char* name_;
  int cur_mode_id_;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_MONITOR_H
