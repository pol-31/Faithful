#ifndef FAITHFUL_CURSOR_H
#define FAITHFUL_CURSOR_H

#include <memory>

#include <glad/gl.h>

#include "Window.h"

namespace faithful {
namespace details {
namespace io {

class DefaultCursor;

class Cursor {
 public:
  Cursor(GLFWwindow* glfw_window);
  ~Cursor();

  Cursor(GLFWwindow* glfw_window, std::unique_ptr<uint8_t[]> data,
         int width, int height);

  // conversion for  usage in GLFW-functions
  GLFWcursor* Glfw() {
    return cursor_;
  }

  void MakeCursorVisible();
  void MakeCursorUnvisible();

  void MakeActive();

 protected:
  friend class DefaultCursor;
  void set_cursor(GLFWcursor* cursor) {
    cursor_ = cursor;
  }

 private:
  GLFWwindow* glfw_window_;
  GLFWcursor* cursor_ = nullptr;
  bool visible_ = true;
  bool active_ = false;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_CURSOR_H
