#ifndef FAITHFUL_SRC_COMMON_GLOBALSTATEAWAREBASE_H_
#define FAITHFUL_SRC_COMMON_GLOBALSTATEAWAREBASE_H_

#include "GlobalStateInfo.h"

namespace faithful {
namespace details {

class GlobalStateAwareBase {
 public:
  GlobalStateAwareBase() = default;
  GlobalStateAwareBase(const GlobalStateAwareBase&) = default;
  GlobalStateAwareBase& operator=(const GlobalStateAwareBase&) = default;
  GlobalStateAwareBase(GlobalStateAwareBase&&) = default;
  GlobalStateAwareBase& operator=(GlobalStateAwareBase&&) = default;

  void SetGlfwWindowUserPointer(GlobalStateInfo* glfw_window_user_pointer) {
    global_state_info_ = glfw_window_user_pointer;
  }

 protected:
  GlobalStateInfo* global_state_info_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_GLOBALSTATEAWAREBASE_H_
