#ifndef FAITHFUL_MONITOR_H
#define FAITHFUL_MONITOR_H

#include <string>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <iostream>

#include "../../utils/ConstexprVector.h"

namespace faithful {
namespace details {
namespace io {

class Monitor {
 public:
  struct ModeInfo {
    int width;
    int height;
    int refresh_rate;
  };

  Monitor(GLFWmonitor* glfw_monitor,
          faithful::utils::ConstexprVector<const GLFWvidmode*, 10> modes)
      : modes_(modes), glfw_monitor_(glfw_monitor) {
    glfw_monitor_ = glfwGetPrimaryMonitor();
    name_ = glfwGetMonitorName(glfw_monitor_);
    const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitor_);
    // TODO: cur_mode = highest possible
  }

  glm::vec2 GetResolution() const {
    return {cur_mode_.width, cur_mode_.height};
  }

  void SetMode(int id) {
    cur_mode_ = mode;
  }

 private:
  //TODO:
  // Monitor should keep SOLELY pointers to data stored by GLFW
  // we don't need allocate smt, just provide "cached storage" and
  // convenient output

  faithful::utils::ConstexprVector<const GLFWvidmode*, 10> modes_;
  int cur_mode_id_;
  GLFWmonitor* glfw_monitor_;
  const char* name_;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_MONITOR_H
