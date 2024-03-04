#ifndef FAITHFUL_WINDOW_H
#define FAITHFUL_WINDOW_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "MonitorsInfo.h"

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace io {

// Currently it's one full-screen window, can't be run in windowed mode
/* TODO: de we actually need windowed-mode?
    close/iconify;
    fullscreen (on/off);
    rewrite CalculateDefaultResolution();
    refactor MonitorsInfo.h - MonitorsInfo::glfw_window_
    + glfw window-related callbacks (resize / iconified, close, content_scale)
 * */

class Window {
 public:
  Window();
  ~Window();

  const glm::ivec2& GetResolution() {
    return resolution_;
  }

  void SetMonitor(int monitor_id);
  void SetResolution(int monitor_mode_id);

  GLFWwindow* Glfw() {
    return glfw_window_;
  }

  void Update();

  void EnableVSync();
  void DisableVSync();

 protected:
  friend class faithful::details::DisplayInteractionThreadPool;
  MonitorsInfo& GetMonitorInfoRef() {
    return monitors_info_;
  }

 private:
  void Init();
  void DeInit();

  glm::ivec2 CalculateDefaultResolution();

  GLFWwindow* glfw_window_ = nullptr;
  MonitorsInfo monitors_info_;
  glm::ivec2 resolution_;
  bool enable_vsync_ = false;
};

} // namespace io
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_WINDOW_H
