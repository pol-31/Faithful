#include "InputModule.h"

#include "../common/GlfwCallbackData.h"

#include "../../assets/embedded/CursorMainMenu.h"
#include "../../assets/embedded/CursorMainGame.h"

namespace faithful {
namespace details {

InputModule::InputModule(
    io::Window& window)
    : window_(window),
      cursor_arrow_(faithful::embedded::kCursorMainMenuData,
                    faithful::embedded::kCursorMainMenuWidth,
                    faithful::embedded::kCursorMainMenuHeight),
      cursor_target_(faithful::embedded::kCursorMainGameData,
                     faithful::embedded::kCursorMainGameWidth,
                     faithful::embedded::kCursorMainGameHeight),
      current_cursor_(&cursor_arrow_) {}

void MenuKeyCallback(GLFWwindow* window, int key,
                     int scancode __attribute__((unused)),
                     int action, int mods __attribute__((unused))) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    auto p_glfw_callback_data = reinterpret_cast<GlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    p_glfw_callback_data->PressKey(key);
  }
}

void MenuMouseButtonCallback(GLFWwindow* window,
                             int button __attribute__((unused)),
                             int action, int mods __attribute__((unused))) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    auto p_glfw_callback_data = reinterpret_cast<GlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    p_glfw_callback_data->PressKey(GLFW_KEY_ENTER);
  }
}

void MenuScrollCallback(GLFWwindow* window,
                        double xoffset, double yoffset) {
  /// wasd more common than up-left-down-right
  auto p_glfw_callback_data = reinterpret_cast<GlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  if (yoffset > 0) {
    p_glfw_callback_data->PressKey(GLFW_KEY_W);
  } else if (yoffset < 0) {
    p_glfw_callback_data->PressKey(GLFW_KEY_S);
  } else if (xoffset > 0) {
    p_glfw_callback_data->PressKey(GLFW_KEY_D);
  } else {
    p_glfw_callback_data->PressKey(GLFW_KEY_A);
  }
}

void InputModule::SetupMenu() {
  glfwSetMouseButtonCallback(window_.Glfw(), MenuMouseButtonCallback);
  glfwSetScrollCallback(window_.Glfw(), MenuScrollCallback);
  glfwSetKeyCallback(window_.Glfw(), MenuKeyCallback);
}

void InputModule::SetupGame() {
  //
}

glm::vec2 InputModule::GetCursorPos() {
  double cursor_pos_x, cursor_pos_y;
  glfwGetCursorPos(window_.Glfw(), &cursor_pos_x, &cursor_pos_y);
  return {cursor_pos_x, cursor_pos_y};
}

} // namespace details
} // namespace faithful
