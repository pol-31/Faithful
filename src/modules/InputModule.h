#ifndef FAITHFUL_SRC_MODULES_INPUTMODULE_H_
#define FAITHFUL_SRC_MODULES_INPUTMODULE_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../io/Window.h"
#include "../io/Cursor.h"

namespace faithful {
namespace details {

void MenuKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

void MenuMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);
void MenuScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

class InputModule {
 public:
  InputModule() = delete;
  InputModule(io::Window& window);

  void SetupMenu();
  void SetupGame();

  glm::vec2 GetCursorPos();

 private:
  io::Window& window_;
  faithful::details::io::Cursor cursor_arrow_;
  faithful::details::io::Cursor cursor_target_;
  faithful::details::io::Cursor* current_cursor_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_INPUTMODULE_H_
