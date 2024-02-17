#include "Camera.h"
#include "../MainRunLoop.h"
#include "../Scene.h"

namespace faithful {

Camera::Camera() {
  cur_id_ = ++id_;
  transform_ = new glm::mat4(1.0f);
}

Camera::Camera(const Camera& camera) noexcept {
}  // TODO:

Camera::Camera(Camera&&) noexcept {
}  // TODO:

Camera::Camera(SceneMode mode) {
  transform_ = new glm::mat4(1.0f);
  switch (mode) {
    case SceneMode::Shooter:
      keyboard_handler_ = new KeyboardInputHandler(SceneMode::Shooter);
      mouse_handler_ = new MouseInputHandler(SceneMode::Shooter);
      break;
    case SceneMode::Rpg:
      keyboard_handler_ = new KeyboardInputHandler(SceneMode::Rpg);
      mouse_handler_ = new MouseInputHandler(SceneMode::Rpg);
      break;
    case SceneMode::Strategy:
      keyboard_handler_ = new KeyboardInputHandler(SceneMode::Strategy);
      mouse_handler_ = new MouseInputHandler(SceneMode::Strategy);
      break;
    case SceneMode::Platformer:
      keyboard_handler_ = new KeyboardInputHandler(SceneMode::Platformer);
      mouse_handler_ = new MouseInputHandler(SceneMode::Platformer);
      break;
  }
}

Camera& Camera::operator=(const Camera& camera) noexcept {
}  // TODO:
Camera& Camera::operator=(Camera&& camera) noexcept {
}  // TODO:

// Is non-constexpr because of UpdateVectors
Camera::Camera(glm::vec3 position, float pitch, float yaw) noexcept
    : pitch_(pitch),
      yaw_(yaw) {
  transform_ = new glm::mat4(1.0f);
  move_speed_ = 5.0f;
  sensitivity_ = 0.1f;
  TranslateTo(0.0f, 0.0f, 5.0f);
  UpdateVectors();
}

void Camera::Move(Camera::MoveDirection direction) noexcept {
  double frame_time = SimurghManager::get_framerate();
  float velocity = move_speed_ * frame_time;
  switch (direction) {
    case MoveDirection::Forward:
      TranslateOn(direction_front_ * velocity);
      break;
    case MoveDirection::Back:
      TranslateOn(-direction_front_ * velocity);
      break;
    case MoveDirection::Left:
      TranslateOn(direction_right_ * velocity);
      break;
    case MoveDirection::Right:
      TranslateOn(-direction_right_ * velocity);
      break;
    case MoveDirection::Up:
      TranslateOn(direction_up_ * velocity);
      break;
    case MoveDirection::Down:
      TranslateOn(-direction_up_ * velocity);
      break;
  }
  // TODO: take constraints in current class_Scene
  // position_.y = 0;
}

// Is non-constexpr because of UpdateVectors
void Camera::Look(float offset_x, float offset_y) noexcept {
  yaw_ += offset_x * sensitivity_;
  pitch_ += offset_y * sensitivity_;
  if (pitch_ > 89.5f) {
    pitch_ = 89.5f;
  } else if (pitch_ < -89.5f) {
    pitch_ = -89.5f;
  }
  UpdateVectors();
}

void Camera::Zoom(float offset_y) noexcept {
  zoom_ += offset_y;
  if (zoom_ >= 45.0f) {
    zoom_ = 45.0f;
  } else if (zoom_ <= 1.0f) {
    zoom_ = 1.0f;
  }
}

void Camera::ProcessInput(Window* window) {
  if (mouse_handler_ != nullptr)
    mouse_handler_->Run(window);
  if (keyboard_handler_ != nullptr)
    keyboard_handler_->Run(window);
}

void Camera::UpdateVectors() noexcept {
  glm::vec3 direction_front;
  direction_front.x =
      glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
  direction_front.y = glm::sin(glm::radians(pitch_));
  direction_front.z =
      glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
  direction_front_ = glm::normalize(direction_front);

  direction_right_ = glm::cross(direction_world_up_, direction_front_);
  direction_up_ = glm::cross(direction_front_, direction_right_);

  // TODO: not effective to normalize each frame
  direction_right_ = glm::normalize(direction_right_);
  direction_up_ = glm::normalize(direction_up_);
}

void Camera::MakeActive() {
  // TODO: bind callbacks
  if (mouse_handler_ != nullptr)
    mouse_handler_->ConfigureCursor();
}

int Camera::id_ = 0;

}  // namespace faithful
