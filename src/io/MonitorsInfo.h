#ifndef FAITHFUL_SRC_IO_MONITORSINFO_H_
#define FAITHFUL_SRC_IO_MONITORSINFO_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Monitor.h"

#include "../../utils/ConstexprVector.h"
#include "../../config/IO.h"
#include "../common/GlobalStates.h"

namespace faithful {
namespace details {
namespace io {

void GlfwMonitorCallback(GLFWmonitor* monitor __attribute__((unused)),
                         int event __attribute__((unused))) {
  faithful::global::need_to_update_monitor = true;
}

class MonitorsInfo {
 public:
  using MonitorInfoType =
      faithful::utils::ConstexprVector<Monitor, faithful::config::max_monitors>;

  MonitorsInfo() {
    /// we could set callback once somewhere in main.cpp::main(), but
    /// we can easily forget about it. Creation of MonitorInfo isn't often
    /// operation so there is not overhead. (currently 1 window -> 1 MonitorInfo
    /// and we need only one window)
    glfwSetMonitorCallback(GlfwMonitorCallback);
    Update(glfwGetPrimaryMonitor());
  }

  void Update(GLFWmonitor* cur_monitor) {
    monitors_info_.Clear();
    GLFWmonitor* monitor = nullptr;
    std::array<Monitor, 3> monitors_{monitor};
    int monitors_num;
    GLFWmonitor** monitors = glfwGetMonitors(&monitors_num);
    for (int i = 0; i < monitors_num; ++i) {
      monitors_info_.PushBack(Monitor(monitors[i]));
    }
    /// deducing of cur_monitor_id_
    const char* cur_monitor_name = glfwGetMonitorName(cur_monitor);
    glm::ivec2 cur_monitor_pos;
    glm::ivec2 candidate_monitor_pos;
    glfwGetMonitorPos(cur_monitor, &cur_monitor_pos.x, &cur_monitor_pos.y);
    // comparing by name and virtual position should be enough,
    // otherwise we'll get std::terminate() (?)
    for (MonitorInfoType::SizeType i = 0; i < monitors_info_.Size(); ++i) {
      if (std::strcmp(glfwGetMonitorName(monitors_info_[i].Glfw()),
                      cur_monitor_name)) {
        glfwGetMonitorPos(cur_monitor, &candidate_monitor_pos.x,
                          &candidate_monitor_pos.y);
        if (cur_monitor_pos == candidate_monitor_pos) {
          cur_monitor_id_ = i;
          return;
        }
      }
    }
    // TODO: FAITHFUL_UNREACHABLE
    std::cerr << "MonitorInfo::Update() can't deduce current monitor" << std::endl;
    std::terminate();
  }

  void SetWindowMonitor(GLFWwindow* glfw_window, int monitor_id) {
    monitors_info_[monitor_id].SetMonitor(glfw_window);
  }
  void SetWindowMonitorMode(GLFWwindow* glfw_window, int mode_id) {
    monitors_info_[cur_monitor_id_].SetMode(glfw_window, mode_id);
  }

  const MonitorInfoType& GetMonitorsInfo() const {
    return monitors_info_;
  }
  int GetCurMonitorId() {
    return cur_monitor_id_;
  }

  Monitor& GetCurMonitor() {
    return monitors_info_[cur_monitor_id_];
  }

 private:
  MonitorInfoType monitors_info_;
  int cur_monitor_id_;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_IO_MONITORSINFO_H_
