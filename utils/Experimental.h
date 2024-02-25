#ifndef FAITHFUL_UTILS_EXPERIMENTAL_H_
#define FAITHFUL_UTILS_EXPERIMENTAL_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace faithful {
namespace experimental {

class Transformable3D {
 public:
  Transformable3D() = default;

  glm::vec3 GetPosition() const;

  glm::quat GetRotationQuat() const;
  glm::vec3 GetRotationRad() const;

  glm::vec3 GetScale() const;

  /// positioning
  void TranslateTo(float x, float y, float z);
  void TranslateTo(const glm::vec3& pos);

  void TranslateOn(float x, float y, float z);
  void TranslateOn(const glm::vec3& delta);

  void TranslateOnRelated(float len, float origin_x,
                          float origin_y, float origin_z);
  void TranslateOnRelated(float len, const glm::vec3& origin_pos);

  /// rotation
  void RotateTo(float radians, const glm::vec3& axis);
  void RotateTo(const glm::quat& rot, const glm::vec3& axis);

  void RotateOn(float radians, const glm::vec3& axis);
  void RotateOn(const glm::quat& rot, const glm::vec3& axis);

  void RotateOnRelated(const glm::quat& rot, float origin_x,
                       float origin_y, float origin_z);
  void RotateOnRelated(const glm::quat& rot, const glm::vec3& origin_pos);

  /// scale
  void ScaleTo(float factor);
  void ScaleTo(float x, float y, float z);
  void ScaleTo(const glm::vec3& scale);

  void ScaleOn(float diff);
  void ScaleOn(float x, float y, float z);
  void ScaleOn(const glm::vec3& scale);

  void ScaleOnRelated(float factor, float origin_x,
                      float origin_y, float origin_z);
  void ScaleOnRelated(float factor, const glm::vec3& origin_pos);

 protected:
  void RotateOnRelatedTranslation(const glm::quat& rot,
                                  const glm::vec3& origin_pos);

  glm::mat4 transform_;
};

/// Transformable3D

glm::vec3 Transformable3D::GetPosition() const {
  return glm::vec3(transform_[2]);
}

glm::quat Transformable3D::GetRotationQuat() const {
  return glm::quat_cast(transform_);
}

glm::vec3 Transformable3D::GetRotationRad() const {
  float angle_x = std::atan2(-transform_[2][1],
                             std::sqrt(transform_[2][0] * transform_[2][0] +
                                       transform_[2][2] * transform_[2][2]));
  float angle_y = std::atan2(transform_[2][0], transform_[2][2]);
  float angle_z = std::atan2(transform_[1][0], transform_[0][0]);
  return {angle_x, angle_y, angle_z};
}

glm::vec3 Transformable3D::GetScale() const {
  return {transform_[0][0], transform_[1][1], transform_[2][2]};
}

/// positioning
void Transformable3D::TranslateTo(float x, float y, float z) {
  transform_[2] = glm::vec4(x, y, z, 1.0f);
}
void Transformable3D::TranslateTo(const glm::vec3& pos) {
  transform_[2] = glm::vec4(pos, 1.0f);
}

void Transformable3D::TranslateOn(float x, float y, float z) {
  transform_ = glm::translate(transform_, {x, y, z});
}
void Transformable3D::TranslateOn(const glm::vec3& delta) {
  transform_ = glm::translate(transform_, delta);
}

void Transformable3D::TranslateOnRelated(float len, float origin_x,
                                         float origin_y, float origin_z) {
  glm::vec3 direction = GetPosition() - glm::vec3(origin_x, origin_y, origin_z);
  transform_[2] = glm::vec4(len * direction, 1.0f);
}
void Transformable3D::TranslateOnRelated(float len, const glm::vec3& origin_pos) {
  glm::vec3 direction = GetPosition() - origin_pos;
  transform_[2] = glm::vec4(len * direction, 1.0f);
}

/// rotation
void Transformable3D::RotateTo(float radians, const glm::vec3& axis) {
  glm::vec3 scale = GetScale();
  glm::vec3 pos = GetPosition();
  transform_ = glm::rotate(glm::mat4(1.0f), radians, axis);
  ScaleTo(scale);
  TranslateTo(pos);
}
void Transformable3D::RotateTo(const glm::quat& rot, const glm::vec3& axis) {
  glm::vec3 scale = GetScale();
  glm::vec3 pos = GetPosition();
  glm::mat4 rotation_matrix = glm::mat4_cast(rot);
  transform_ = glm::rotate(transform_, glm::angle(rot), axis);
  ScaleTo(scale);
  TranslateTo(pos);
}

void Transformable3D::RotateOn(float radians, const glm::vec3& axis) {
  transform_ = glm::rotate(transform_, radians, axis);
}
void Transformable3D::RotateOn(const glm::quat& rot, const glm::vec3& axis) {
  glm::mat4 rotation_matrix = glm::mat4_cast(rot);
  transform_ = glm::rotate(transform_, glm::angle(rot), axis);
}

void Transformable3D::RotateOnRelated(const glm::quat& rot, float origin_x,
                                      float origin_y, float origin_z) {
  RotateOn(rot, {1.0f, 1.0f, 1.0f});
  RotateOnRelatedTranslation(rot, {origin_x, origin_y, origin_z});
}
void Transformable3D::RotateOnRelated(const glm::quat& rot, const glm::vec3& origin_pos) {
  RotateOn(rot, {1.0f, 1.0f, 1.0f});
  RotateOnRelatedTranslation(rot, origin_pos);
}

/// scale
void Transformable3D::ScaleTo(float factor) {
  transform_[0][0] = factor;
  transform_[1][1] = factor;
  transform_[2][2] = factor;
}
void Transformable3D::ScaleTo(float x, float y, float z) {
  transform_[0][0] = x;
  transform_[1][1] = y;
  transform_[2][2] = z;
}
void Transformable3D::ScaleTo(const glm::vec3& scale) {
  transform_[0][0] = scale.x;
  transform_[1][1] = scale.y;
  transform_[2][2] = scale.z;
}

void Transformable3D::ScaleOn(float diff) {
  transform_ = glm::scale(transform_, {diff, diff, diff});
}
void Transformable3D::ScaleOn(float x, float y, float z) {
  transform_ = glm::scale(transform_, {x, y, z});
}
void Transformable3D::ScaleOn(const glm::vec3& scale) {
  transform_ = glm::scale(transform_, scale);
}

void Transformable3D::ScaleOnRelated(float factor, float origin_x,
                                     float origin_y, float origin_z) {
  TranslateOnRelated(factor, origin_x, origin_y, origin_z);
  ScaleOn(factor);
}
void Transformable3D::ScaleOnRelated(float factor, const glm::vec3& origin_pos) {
  TranslateOnRelated(factor, origin_pos);
  ScaleOn(factor);
}


void Transformable3D::RotateOnRelatedTranslation(const glm::quat& rot,
                                                 const glm::vec3& origin_pos) {
  glm::vec3 direction = GetPosition() - origin_pos;
  glm::vec3 rotated_direction = glm::mat3_cast(rot) * direction;

  float len = glm::length(rotated_direction);
  float beta = glm::pi<float>() - glm::acos(rotated_direction.x / len);

  float beta_cos = glm::cos(beta);
  float beta_sin = glm::sin(beta);

  float hypotenuse = 2 * len * len * (1 - beta_cos);
  glm::vec3 new_position = origin_pos + glm::vec3(hypotenuse * beta_sin,
                                                  0.0f, hypotenuse * beta_cos);
  TranslateTo(new_position);
}


} // namespace experimental
} // namespace faithful

#endif  // FAITHFUL_UTILS_EXPERIMENTAL_H_
