#ifndef FAITHFUL_SRC_IO_MONITORINFO_H_
#define FAITHFUL_SRC_IO_MONITORINFO_H_

#include <optional>
#include <string>
#include <vector> // replace by ConstexprVector

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <iostream>

#include "Monitor.h"
#include "../../utils/ConstexprVector.h"
#include "../../config/IO.h"

#include "../../utils/Span.h"

namespace faithful {
namespace details {
namespace io {

class MonitorInfo {
 public:
  static Monitor GetDefaultMonitor() {
  }

  static void Update() {
    monitors_info_.Clear();
    int monitor_num;
    GLFWmonitor** monitors = glfwGetMonitors(&monitor_num);
    for (int i = 0; i < monitor_num; ++i) {
//      Monitor(
      monitors_info_.PushBack(monitors[i]);
    }
  }

  // TODO: we need mdSpan there ?
  const faithful::utils::Span<const GLFWvidmode*>& GetMonitorsInfo() const {
    return monitors_info_;
  }

 private:
  faithful::utils::Span<const GLFWvidmode*> monitors_info_;
};

void monitor_callback(GLFWmonitor* monitor, int event) {
  MonitorInfo::Update(); // OR switch to some active
}

// TODO: glfwSetMonitorCallback() to "update" if user already was in settings


}  // namespace io
}  // namespace details
}  // namespace faithful


#endif  // FAITHFUL_SRC_IO_MONITORINFO_H_
