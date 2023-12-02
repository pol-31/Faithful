#ifndef FAITHFUL_CAMERA_H
#define FAITHFUL_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../entities/Object.h"
#include "InputHandler.h"

namespace faithful {

enum class SceneMode;

// TODO: my own GLM library (constexpr + noexcept)
//        because GLM is not constexpr

/*

  if (glfwRawMouseMotionSupported()) { // TODO: wtf is this
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GL_TRUE);
    // IDK what is this, so better to find out firstly
  }
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // depends on Camera mode

int lastX = 250; // depends on Camera mode
int lastY = 250; // depends on Camera mode
*/

/**
 * ____________________DEFAULT CAMERA MODES
 * MOVE-types:
 * - WASD (front-left-back-right) << rpg\shooter\cosmos
 * - car (left/right impossible with zero velocity)
 * - 2d (may or may not depends on Scene, so Scene is friend class
 *      rectangle/circle in the centre of screen (then camera moves only when
 *      the centre of the screen is outside this scope))
 * LOOK-type:
 * - 1-th (camera pos = binded obj pos)
 * - 3-th (camera pos = binded obj pos + Radius)
 * - cursor
 * */


class Camera : public Object3D {
 public:
  enum class MoveDirection {
    Forward,
    Back,
    Left,
    Right,
    Up,
    Down
  };

  Camera();

  Camera(const Camera &camera) noexcept;
  Camera(Camera &&) noexcept;

  Camera(SceneMode mode);

  Camera(glm::vec3 position, float pitch = 0.0f,
         float yaw = -90.0f) noexcept;

  Camera &operator=(const Camera &camera) noexcept;
  Camera &operator=(Camera &&camera) noexcept;

  void Look(float offset_x, float offset_y) noexcept;
  void Move(Camera::MoveDirection direction) noexcept;
  void Zoom(float offset_y) noexcept;

  /// CreateViewMatrix
  // non-constexpr because of non-constexpresness of glm::LookAt
  // In current context View matrix is the same as LookAt mathix
  glm::mat4 CreateViewMatrix() const noexcept {
    return glm::lookAt(get_position(), get_position() + direction_front_,
                       direction_up_);
  }

  // TODO: make castomisable
  /// CreateProjectionMatrix
  glm::mat4 CreateProjectionMatrix() const noexcept {
    return glm::perspective(glm::radians(get_zoom()),
                            1.0f, 0.1f, 100.0f);
  };

  void ProcessInput(Window* window);

  KeyboardInputHandler* get_keyboard_handler() {
    return keyboard_handler_;
  }
  MouseInputHandler* get_mouse_handler() {
    return mouse_handler_;
  }

  float get_move_speed() const noexcept {
    return move_speed_;
  }

  void set_move_speed(float moveSpeed) noexcept {
    move_speed_ = moveSpeed;
  }

  float get_sensitivity() const noexcept {
    return sensitivity_;
  }

  void set_sensitivity(float sensitivity) noexcept {
    sensitivity_ = sensitivity;
  }

  float get_zoom() const noexcept {
    return zoom_;
  }

  void set_zoom(float zoom) noexcept {
    zoom_ = zoom;
  }

  float get_pitch() const noexcept {
    return pitch_;
  }

  void set_pitch(float pitch) noexcept {
    pitch_ = pitch;
  }

  float get_yaw() const noexcept {
    return yaw_;
  }

  void set_yaw(float yaw) noexcept {
    yaw_ = yaw;
  }

  const glm::vec3 &get_direction_front() const noexcept {
    return direction_front_;
  }

  void set_direction_front(const glm::vec3 &directionFront) noexcept {
    direction_front_ = directionFront;
  }

  const glm::vec3 &get_direction_right() const noexcept {
    return direction_right_;
  }

  void set_direction_right(const glm::vec3 &directionRight) noexcept {
    direction_right_ = directionRight;
  }

  const glm::vec3 &get_direction_up() const noexcept {
    return direction_up_;
  }

  void set_direction_up(const glm::vec3 &directionUp) noexcept {
    direction_up_ = directionUp;
  }

  const glm::vec3 &get_direction_world_up() const noexcept {
    return direction_world_up_;
  }

  void set_direction_world_up(const glm::vec3 &directionWorldUp) noexcept {
    direction_world_up_ = directionWorldUp;
  }

  void set_handler(KeyboardInputHandler* input_handler) {
    keyboard_handler_ = input_handler;
  }
  void set_handler(MouseInputHandler* input_handler) {
    mouse_handler_ = input_handler;
  }
  void set_handler(
      KeyboardInputHandler* keyboard_handler,
      MouseInputHandler* mouse_handler) {
    keyboard_handler_ = keyboard_handler;
    mouse_handler_ = mouse_handler;
  }
  void set_handler(
      MouseInputHandler* mouse_handler,
      KeyboardInputHandler* keyboard_handler) {
    keyboard_handler_ = keyboard_handler;
    mouse_handler_ = mouse_handler;
  }

  void MakeActive();

  int cur_id_;

 protected:

  static int id_;

  void UpdateVectors() noexcept;

  float zoom_ = 45.0f;
  float pitch_ = 0.0f;
  float yaw_ = -90.0f;

  glm::vec3 direction_front_ = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 direction_right_ = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::vec3 direction_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 direction_world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);

  float move_speed_ = 5;
  float sensitivity_ = 0.1f;

  KeyboardInputHandler* keyboard_handler_ = nullptr;
  MouseInputHandler* mouse_handler_ = nullptr;
};

} // namespace faithful

#endif // FAITHFUL_CAMERA_H
