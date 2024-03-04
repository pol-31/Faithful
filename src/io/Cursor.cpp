#include "Cursor.h"

namespace faithful {
namespace details {
namespace io {

Cursor::Cursor() {
  glfw_cursor_ = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
}

Cursor::~Cursor() {
  glfwDestroyCursor(glfw_cursor_);
}

Cursor::Cursor(Cursor&& other) {
  glfw_cursor_ = other.glfw_cursor_;
  other.glfw_cursor_ = nullptr;
}
Cursor& Cursor::operator=(Cursor&& other) {
  glfwDestroyCursor(other.glfw_cursor_);
  glfw_cursor_ = other.glfw_cursor_;
  other.glfw_cursor_ = nullptr;
}

Cursor::Cursor(const uint8_t* data, int width, int height) {
  if (!data) {
    glfw_cursor_ = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    return;
  }
  GLFWimage image;
  image.width = width;
  image.height = height;
  // glfwCreateCursor takes image as a const& and guarantee pixel data is copied:
  // """
  // Pointer lifetime:
  //    The specified image data is copied before this function returns.
  // """
  // so we can use const cast here and data remains valid
  image.pixels = const_cast<unsigned char*>(data);
  glfw_cursor_ = glfwCreateCursor(&image, 0, 0);
}


void Cursor::MakeCursorVisible(Window* window) {
  if (active_) {
    glfwSetInputMode(window->Glfw(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  visible_ = true;
}
void Cursor::MakeCursorInvisible(Window* window) {
  visible_ = false;
  if (active_) {
    glfwSetInputMode(window->Glfw(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

void Cursor::MakeActive(Window* window) {
  if (visible_) {
    glfwSetInputMode(window->Glfw(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(window->Glfw(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  glfwSetCursor(window->Glfw(), glfw_cursor_);
}

}  // namespace io
}  // namespace details
}  // namespace faithful
