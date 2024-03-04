#include "MonitorsInfo.h"

#include "Window.h"

namespace faithful {
namespace details {
namespace io {

void GlfwMonitorCallback(GLFWmonitor* monitor,
                         int event __attribute__((unused))) {
  void* data = glfwGetMonitorUserPointer(monitor);
  bool* need_to_update_monitor_ = reinterpret_cast<bool*>(data);
  *need_to_update_monitor_ = true;
}

MonitorsInfo::MonitorsInfo() {
  /// we could set callback once somewhere in main.cpp::main(), but
  /// we can easily forget about it. Creation of MonitorInfo isn't often
  /// operation so there is not overhead. (currently 1 window -> 1 MonitorInfo
  /// and we need only one window)
  glfwSetMonitorCallback(GlfwMonitorCallback);
  Update(glfwGetPrimaryMonitor());
}

void MonitorsInfo::InitUpdateRef(void* update_ref) {
  update_ref_ = update_ref;
  for (MonitorInfoType::SizeType i = 0; i < monitors_info_.Size(); ++i) {
    glfwSetMonitorUserPointer(monitors_info_[i].Glfw(), update_ref);
  }
}

void MonitorsInfo::Update(GLFWmonitor* cur_monitor) {
  monitors_info_.Clear();
  GLFWmonitor* monitor = nullptr;
  std::array<Monitor, 3> monitors_{monitor};
  int monitors_num;
  GLFWmonitor** monitors = glfwGetMonitors(&monitors_num);
  for (int i = 0; i < monitors_num; ++i) {
    monitors_info_.PushBack(Monitor(monitors[i]));
  }

  if (update_ref_) {
    for (MonitorInfoType::SizeType i = 0; i < monitors_info_.Size(); ++i) {
      glfwSetMonitorUserPointer(monitors_info_[i].Glfw(), update_ref_);
    }
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

void MonitorsInfo::SetWindowMonitor(Window* window, int monitor_id) {
  monitors_info_[monitor_id].SetMonitor(window);
}
void MonitorsInfo::SetWindowMonitorMode(Window* window, int mode_id) {
  monitors_info_[cur_monitor_id_].SetMode(window, mode_id);
}

}  // namespace io
}  // namespace details
}  // namespace faithful
