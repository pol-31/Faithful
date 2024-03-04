#ifndef FAITHFUL_CAMERA_H
#define FAITHFUL_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../config/IO.h"

namespace faithful {

/** Camera
 * Basically always on the same position, only Look-At matrix changes.
 * To change Look-At matrix we can Rotate() or Zoom(), where rotation
 * affects only x/y coordinates and they both aware of:
 * x : y : z always const.
 * */

/// It isn't inherited from Positionable3D although has single glm::mat4.
/// We did it because it has slightly different purpose and mechanic

/// speed of Zoom/Rotate/Move is managed externally, so
/// there's only immediate transformations

class Camera {
 public:
  Camera(const glm::ivec2& screen_resolution)
      : position_(faithful::config::kCameraDirPosition),
        screen_resolution_(screen_resolution),
        yaw_(faithful::config::kCameraYaw) {
    UpdateMatrices();
  }

  Camera(const Camera&) = default;
  Camera(Camera&&) = default;

  Camera& operator=(const Camera& camera) = delete;
  Camera& operator=(Camera&& camera) = delete;

  void Zoom(float factor) {
    position_ *= factor;
    UpdateMatrices();
  }
  void Rotate(float radians) {
    yaw_ = radians;
    UpdateMatrices();
  }
  /// coord_y always stay at the same level (we want such Camera mechanic)
  void Move(float coord_x, float coord_z) {
    position_.x = coord_x;
    position_.z = coord_z;
    UpdateMatrices();
  }

  /// we return references, so don't need to retrieve it each frame
  const glm::mat4& GetLookAtMatrix() const {
    return look_at_matrix_;
  }
  const glm::mat4& GetProjectionMatrix() const {
    return look_at_matrix_;
  }

  /// useful if resolution has been changed and we need to update our matrices
  void Update() {
    UpdateMatrices();
  }

 private:
  void UpdateMatrices() {
    glm::vec3 direction_front;
    direction_front.x =
        glm::cos(glm::radians(faithful::config::kCameraYaw))
        * glm::cos(glm::radians(faithful::config::kCameraPitch));
    direction_front.y = glm::sin(glm::radians(faithful::config::kCameraPitch));
    direction_front.z =
        glm::sin(glm::radians(faithful::config::kCameraYaw))
        * glm::cos(glm::radians(faithful::config::kCameraPitch));
    direction_front_ = glm::normalize(direction_front);

    glm::vec3 direction_right = glm::cross(faithful::config::kCameraDirWorldUp,
                                           direction_front_);
    glm::vec3 direction_up = glm::cross(direction_front_, direction_right);
    direction_up_ = glm::normalize(direction_up);

    look_at_matrix_ = glm::lookAt(position_, position_ + direction_front_,
                                  direction_up_);
    auto aspect = static_cast<float>(screen_resolution_.x / screen_resolution_.y);
    projection_matrix_ = glm::perspective(
        faithful::config::kCameraFov, aspect,
        faithful::config::kCameraPerspectiveNear,
        faithful::config::kCameraPerspectiveFar);
  }

  glm::mat4 look_at_matrix_;
  glm::mat4 projection_matrix_;

  glm::vec3 position_;
  glm::vec3 direction_front_;
  glm::vec3 direction_up_;

  const glm::ivec2& screen_resolution_;

  float yaw_;
};

}  // namespace faithful

#endif  // FAITHFUL_CAMERA_H
