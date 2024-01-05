#include "Cursor.h"
#include <cstring>

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../Engine.h"
#include "../../utils/Executors/ThreadPoolCall.h"
#include "../../utils/Allocator.h"

namespace faithful {

Cursor::Cursor() {
  related_window_ = CurrentWindow();
}

Cursor::Cursor(Cursor&& other) {
  if (cursor_ != nullptr)
    delete cursor_;
  cursor_ = other.cursor_;
  visible_ = other.visible_;
  related_window_ = other.related_window_;

  other.cursor_ = nullptr;
}

Cursor::Cursor(SceneMode mode) {
  related_window_ = CurrentWindow();
  switch (mode) {
    case SceneMode::Shooter:
      visible_ = false;
      break;
    case SceneMode::Rpg:
      visible_ = false;
      break;
    case SceneMode::Strategy:
      SingleSemiDeferredCall{}.Do([=]() {
        utility::Span<unsigned char> buffer(16 * 16 * 4);
        std::memset(buffer.get_data(), 0xff, buffer.get_size());
        GLFWimage image;
        image.width = 16;
        image.height = 16;
        image.pixels = buffer.get_data();
        cursor_ = glfwCreateCursor(&image, 0, 0);
      });
      visible_ = true;
      break;
    case SceneMode::Platformer:
      visible_ = false;
      break;
  }
}

Cursor& Cursor::operator=(Cursor&& other) {
  if (cursor_ != nullptr)
    delete cursor_;
  cursor_ = other.cursor_;
  visible_ = other.visible_;
  related_window_ = other.related_window_;

  other.cursor_ = nullptr;
  return *this;
}

Cursor::~Cursor() {
  SingleSemiDeferredCall{}.Do([=]() {
    glfwDestroyCursor(cursor_);
  });
}

void Cursor::MakeCursorVisible() {
  visible_ = true;
}
void Cursor::MakeCursorUnvisible() {
  visible_ = false;
}

void Cursor::MakeCursorVisibleImpl() {
  SingleSemiDeferredCall{}.Do([=]() {
    glfwSetInputMode(related_window_->Glfw(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  });
}
void Cursor::MakeCursorUnvisibleImpl() {
  SingleSemiDeferredCall{}.Do([=]() {
    glfwSetInputMode(related_window_->Glfw(), GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED);
  });
}

void Cursor::MakeActive() {
  if (visible_) {
    MakeCursorVisibleImpl();
  } else {
    MakeCursorUnvisibleImpl();
  }
  SingleSemiDeferredCall{}.Do([=]() {
    glfwSetCursor(related_window_->Glfw(), cursor_);
  });
}

// TODO: default cursor
void DefaultCursor::Init() {
  if (initialized_)
    return;

  utility::Span<unsigned char> buffer(16 * 16 * 4);
  std::memset(buffer.get_data(), 0xff, buffer.get_size());

  GLFWimage image;
  image.width = 16;
  image.height = 16;
  image.pixels = buffer.get_data();
  cursor_ = new Cursor;
  cursor_->set_cursor(glfwCreateCursor(&image, 0, 0));
  initialized_ = true;

  no_cursor_ = new Cursor;
  no_cursor_->MakeCursorUnvisible();
}

bool DefaultCursor::initialized_ = false;
Cursor* DefaultCursor::cursor_ = nullptr;
Cursor* DefaultCursor::no_cursor_ = nullptr;

}  // namespace faithful
