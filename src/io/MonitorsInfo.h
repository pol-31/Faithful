#ifndef FAITHFUL_SRC_IO_MONITORSINFO_H_
#define FAITHFUL_SRC_IO_MONITORSINFO_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Monitor.h"

#include "../../utils/ConstexprVector.h"
#include "../../config/IO.h"

namespace faithful {
namespace details {
namespace io {

class Window;
class Monitor;

void GlfwMonitorCallback(GLFWmonitor* monitor, int event);

class MonitorsInfo {
 public:
  using MonitorInfoType =
      faithful::utils::ConstexprVector<Monitor, faithful::config::kMaxMonitors>;

  MonitorsInfo();

  void InitUpdateRef(void* update_ref);

  void Update(GLFWmonitor* cur_monitor);

  void SetWindowMonitor(Window* window, int monitor_id);
  void SetWindowMonitorMode(Window* window, int mode_id);

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
  void* update_ref_ = nullptr;
  int cur_monitor_id_;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_IO_MONITORSINFO_H_
