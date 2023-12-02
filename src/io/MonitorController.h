#ifndef FAITHFUL_MONITORCONTROLLER_H
#define FAITHFUL_MONITORCONTROLLER_H

#include <algorithm>
#include <optional> // need to get rid of

#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "Monitor.h"


#include <iostream>
namespace faithful {

class MonitorController {
 public:
  MonitorController() {
    Update();
  }

  //
  /// Primary_monitor or window_monitor ????????
  //


  void Update() {
    int monitor_number = 0;
    GLFWmonitor** monitors_p = glfwGetMonitors(&monitor_number);
    if (monitors_ == nullptr) {
      // TODO: custom Alloc/Dealloc
      monitors_ = new std::vector<Monitor>(monitors_p,
                                           monitors_p + monitor_number);
      for (auto m = monitors_->begin(); m != monitors_->end(); ++m) {
        if (*m == glfwGetPrimaryMonitor()) {
          cur_monitor_ = &*m;
          return;
        }
      }
      return;
    }

    // TODO: custom Alloc/Dealloc
    std::vector<GLFWmonitor*> glfw_monitors(monitors_p,
                                            monitors_p + monitor_number);

    Revalidate();
    for (auto m : glfw_monitors) {
      StartTracking(m);
    }
    *cur_monitor_ = Monitor(glfwGetPrimaryMonitor());
  }

  Monitor* Current() {
    return cur_monitor_;
  }

  // TODO: std::optional always has value, so need to refactor somehow
  std::optional<glm::vec2> CurrentMonitorResolution() {
    return cur_monitor_->Resolution();
  }

  std::optional<glm::vec2> CurrentMonitorContentScale() {
    return cur_monitor_->ContentScale();
  }
  const char* CurrentMonitorName() {
    return cur_monitor_->Name();
  }
  std::optional<glm::vec2> CurrentMonitorPhysicalSize() {
    return cur_monitor_->PhysicalSize();
  }
  std::optional<glm::vec2> CurrentMonitorPosition() {
    return cur_monitor_->Position();
  }
  std::optional<glm::vec4> CurrentMonitorWorkarea() {
    return cur_monitor_->Workarea();
  }
  const std::vector<Monitor::Mode>* CurrentMonitorVideoModes() {
    return cur_monitor_->VideoModes();
  }

 private:
  void Revalidate() {
    // TODO: custom std::erase_if (c++20) ((we are c++17))
    /*monitors_->erase(
      std::remove_if(monitors_->begin(), monitors_->end(),
                     [](const Monitor& monitor)
                       { return !monitor.IsValid(); }),
      monitors_->end());*/
  }
  void StartTracking(GLFWmonitor* new_monitor) {
    bool tracked = false;
    for (auto m = monitors_->begin(); m != monitors_->end(); ++m) {
      if (*m == new_monitor) {
        tracked = true;
        break;
      }
    }
    if (!tracked) {
      monitors_->emplace_back(new_monitor);
    }
  }

  // TODO: reserve vector size
  std::vector<Monitor>* monitors_ = nullptr;
  Monitor* cur_monitor_ = nullptr;
};

} // namespace faithful

#endif // FAITHFUL_MONITORCONTROLLER_H
