#include "Cursor.h"

#include <cstddef>

#include "../../utils/Allocator.h"

namespace faithful {
namespace details {
namespace io {

Cursor::Cursor(GLFWwindow* glfw_window) {
  glfw_window_ = glfw_window;
}

Cursor::Cursor(GLFWwindow* glfw_window, std::unique_ptr<uint8_t[]> data,
               int width, int height) {
  glfw_window_ = glfw_window;
  if (!data) {
    cursor_ = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    return;
  }
  GLFWimage image;
  image.width = width;
  image.height = height;
  image.pixels = data.get();
  cursor_ = glfwCreateCursor(&image, 0, 0);
}

Cursor::~Cursor() {
  glfwDestroyCursor(cursor_);
}

void Cursor::MakeCursorVisible() {
  if (active_) {
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  visible_ = true;
}
void Cursor::MakeCursorUnvisible() {
  visible_ = false;
  if (active_) {
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

void Cursor::MakeActive() {
  if (visible_) {
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  glfwSetCursor(glfw_window_, cursor_);
}

}  // namespace io
}  // namespace details
}  // namespace faithful
