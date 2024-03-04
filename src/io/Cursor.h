#ifndef FAITHFUL_CURSOR_H
#define FAITHFUL_CURSOR_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Window.h"

namespace faithful {
namespace details {
namespace io {

class DefaultCursor;

class Cursor {
 public:
  Cursor();

  ~Cursor();

  Cursor(const Cursor&) = delete;
  Cursor& operator=(const Cursor&) = delete;

  Cursor(Cursor&& other);
  Cursor& operator=(Cursor&& other);

  Cursor(const uint8_t* data, int width, int height);

  GLFWcursor* Glfw() {
    return glfw_cursor_;
  }

  void MakeCursorVisible(Window* window);
  void MakeCursorInvisible(Window* window);

  void MakeActive(Window* window);

 protected:
  friend class DefaultCursor;
  void set_cursor(GLFWcursor* cursor) {
    glfw_cursor_ = cursor;
  }

 private:
  GLFWcursor* glfw_cursor_ = nullptr;
  bool visible_ = true;
  bool active_ = false;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_CURSOR_H
