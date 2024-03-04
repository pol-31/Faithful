#ifndef FAITHFUL_SRC_COMMON_FRAMERATE_H_
#define FAITHFUL_SRC_COMMON_FRAMERATE_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace faithful {
namespace details {

class Framerate {
 public:
  Framerate() = default;

  void UpdateFramerate() {
    double current_frame = glfwGetTime();
    delta_ = current_frame - last_;
    last_ = current_frame;
  }

 private:
  double last_; // TODO: is float enough?
  double delta_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_FRAMERATE_H_
