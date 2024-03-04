#ifndef FAITHFUL_MONITOR_H
#define FAITHFUL_MONITOR_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../utils/Span.h"

namespace faithful {
namespace details {
namespace io {

class Window;

class Monitor {
 public:
  using ModesType = faithful::utils::Span<const GLFWvidmode>;

  Monitor() : Monitor(glfwGetPrimaryMonitor()) {}

  Monitor(const Monitor&) = default;
  Monitor& operator=(const Monitor&) = default;

  Monitor(Monitor&&) = default;
  Monitor& operator=(Monitor&&) = default;

  Monitor(GLFWmonitor* glfw_monitor);

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

  void SetMonitor(Window* window);

  // also sets monitor
  void SetMode(Window* window, int mode_id);

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
