#ifndef FAITHFUL_MONITOR_H
#define FAITHFUL_MONITOR_H

#include <optional>
#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <iostream>

namespace faithful {

namespace details {
namespace monitor {

void DefaultMonitorCallback(GLFWmonitor* monitor, int event);

}  // namespace monitor
}  // namespace details

class Monitor {
 public:
  class Mode {
   public:
    bool operator==(const GLFWvidmode* other) {
      return (other->height == this->height) && (other->width == this->width) &&
             (other->refreshRate == this->framerate);
    }
    std::string name = "";
    int width;
    int height;
    int framerate;
  };

  Monitor() {
  }
  Monitor(GLFWmonitor* monitor) {
    monitor_ = monitor;
    InitVideoModes();

    if (glfwGetMonitorUserPointer(monitor_) != nullptr) {
      owner_ = false;
      return;
    }
    owner_ = true;
    glfwSetMonitorUserPointer(monitor_, this);
    glfwSetMonitorCallback(details::monitor::DefaultMonitorCallback);
    // TODO: to MonitorController
  }

  Monitor(Monitor&& other) {
    monitor_ = other.Glfw();
    InitVideoModes();
    owner_ = std::move(other).StealOwnership();
  }

  Monitor& operator=(Monitor&& other) noexcept {
    monitor_ = other.Glfw();
    InitVideoModes();
    owner_ = std::move(other).StealOwnership();
    return *this;
  }
  Monitor& operator=(GLFWmonitor* monitor) {
    // TODO: check current ownership

    monitor_ = monitor;
    InitVideoModes();

    if (glfwGetMonitorUserPointer(monitor_) != nullptr) {
      owner_ = false;
      return *this;
    }
    owner_ = true;
    glfwSetMonitorUserPointer(monitor_, this);
    glfwSetMonitorCallback(details::monitor::DefaultMonitorCallback);
    // TODO: to MonitorController
  }

  ~Monitor() {
    if (owner_) {
      glfwSetMonitorUserPointer(monitor_, nullptr);
    }
  }

  GLFWmonitor* Glfw() {
    return monitor_;
  }

  bool StealOwnership() && {
    bool ownership = owner_;
    owner_ = false;
    return ownership;
  }

  std::optional<glm::vec2> Resolution() {
    if (!connected_) {
      return {};
    }
    return glm::vec2(mode_->width, mode_->height);
  }

  std::optional<glm::vec2> ContentScale() {
    if (!connected_) {
      return {};
    }
    float xscale, yscale;
    glfwGetMonitorContentScale(monitor_, &xscale, &yscale);
    return glm::vec2(xscale, yscale);
  }

  const char* Name() {
    // works even when monitor is disconnected
    return glfwGetMonitorName(monitor_);
  }

  Mode CurMode() {
    return *mode_;
  }

  std::optional<glm::vec2> PhysicalSize() {
    if (!connected_) {
      return {};
    }
    int width, height;
    glfwGetMonitorPhysicalSize(monitor_, &width, &height);
    return glm::vec2(width, height);
  }

  // for virtual position (where multiple screen are accessible)
  // useless function by now
  std::optional<glm::vec2> Position() {
    if (!connected_) {
      return {};
    }
    int xpos, ypos;
    glfwGetMonitorPos(monitor_, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
  }

  std::optional<glm::vec4> Workarea() {
    if (!connected_) {
      return {};
    }
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(monitor_, &xpos, &ypos, &width, &height);
    return glm::vec4(xpos, ypos, width, height);
  }

  bool IsValid() const {
    return connected_;
  }
  void Connect() {
    connected_ = true;
  }
  void Disconnect() {
    connected_ = false;
  }

  const std::vector<Mode>* VideoModes() {
    return modes_;
  }

  bool operator==(GLFWmonitor* other) {
    return other == monitor_;
  }
  friend bool operator==(Monitor m1, Monitor m2) {
    return m1.monitor_ == m2.monitor_;
  }

  // glfwSetGammaRamp/glfwGetGammaRamp -- is intentionally missing by now
 private:
  void InitVideoModes() {
    int count;
    // we don't use info about RGB and refresh rate from GLFWvidmode,
    // so we can take resolution and its conversion to string, for the purpose
    // of using that window's size that is most desired for user
    const GLFWvidmode* vidmodes = glfwGetVideoModes(monitor_, &count);
    for (int i = 0; i < count; ++i) {
      // TODO: reserve string size
      modes_ = new std::vector<Mode>();

      // TODO: custom Alloc/Dealloc
      std::string name = std::to_string(vidmodes[i].width) + 'x' +
                         std::to_string(vidmodes[i].height) + ' ' +
                         std::to_string(vidmodes[i].refreshRate) + "Hz";
      modes_->push_back(Mode{std::move(name), vidmodes[i].width,
                             vidmodes[i].height, vidmodes[i].refreshRate});

      if (modes_->back() == glfwGetVideoMode(monitor_)) {
        mode_ = &modes_->back();
      }
    }
  }

  GLFWmonitor* monitor_ = nullptr;
  // TODO: reserve vector size
  std::vector<Mode>* modes_ = nullptr;
  Mode* mode_ = nullptr;

  // by default when Monitor is constructed it's connected
  bool connected_ = true;

  // GLFWmonitor* can have the only one true owner
  bool owner_ = false;
};

}  // namespace faithful

#endif  // FAITHFUL_MONITOR_H
