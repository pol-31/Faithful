#include "Monitor.h"

namespace faithful {


namespace details {
namespace monitor {

void DefaultMonitorCallback(GLFWmonitor *monitor, int event) {
  if (event == GLFW_CONNECTED) {
    static_cast<Monitor *>(glfwGetMonitorUserPointer(monitor))->Connect();
  } else if (event == GLFW_DISCONNECTED) {
    static_cast<Monitor *>(glfwGetMonitorUserPointer(monitor))->Disconnect();
  }
}

} // namespace monitor
} // namespace details

} // namespace faithful