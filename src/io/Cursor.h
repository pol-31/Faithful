#ifndef FAITHFUL_CURSOR_H
#define FAITHFUL_CURSOR_H

#include "Window.h"

namespace faithful {
namespace details {
namespace io {

class DefaultCursor;

class Cursor {
 public:
  Cursor() = delete;
  Cursor(Window* window);

  ~Cursor();

  Cursor(const Cursor&) = delete;
  Cursor& operator=(const Cursor&) = delete;

  Cursor(Cursor&& other);
  Cursor& operator=(Cursor&& other);

  Cursor(Window* window, const uint8_t* data,
         int width, int height);

  GLFWcursor* Glfw() {
    return glfw_cursor_;
  }

  void MakeCursorVisible();
  void MakeCursorInvisible();

  void MakeActive();

 protected:
  friend class DefaultCursor;
  void set_cursor(GLFWcursor* cursor) {
    glfw_cursor_ = cursor;
  }

 private:
  Window* window_ = nullptr;
  GLFWcursor* glfw_cursor_ = nullptr;
  bool visible_ = true;
  bool active_ = false;
};

}  // namespace io
}  // namespace details
}  // namespace faithful

#endif  // FAITHFUL_CURSOR_H
