#ifndef FAITHFUL_CURSOR_H
#define FAITHFUL_CURSOR_H

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstddef>

#include "../Scene.h"

namespace faithful {

class DefaultCursor;

class Cursor {
 public:
  Cursor();
  Cursor(Cursor&& other);
  Cursor(SceneMode mode);

  Cursor& operator=(Cursor&& other);

  ~Cursor();

  // conversion for  usage in GLFW-functions
  GLFWcursor* Glfw() {
    return cursor_;
  }

  void MakeCursorVisibleImpl();
  void MakeCursorUnvisibleImpl();

  void MakeCursorVisible();
  void MakeCursorUnvisible();

  void MakeActive();

 protected:
  friend class DefaultCursor;
  void set_cursor(GLFWcursor* cursor) {
    cursor_ = cursor;
  }

 private:
  Window* related_window_ = nullptr;
  GLFWcursor* cursor_ = nullptr;
  bool visible_ = true;
};

class DefaultCursor {
 public:
  static void Init();
  static Cursor* cursor_;
  static Cursor* no_cursor_;

 private:
  static bool initialized_;
};

}  // namespace faithful

#endif  // FAITHFUL_CURSOR_H
