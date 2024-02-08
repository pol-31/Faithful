#include "Object.h"

namespace faithful {

/// Transformable2D

glm::vec2 Transformable2D::GetPosition() const {
  return glm::vec2(transform_[2]);
}

glm::quat Transformable2D::GetRotationQuat() const {
  return glm::quat_cast(transform_);
}
float Transformable2D::GetRotationRad() const {
  return std::atan2(transform_[1][0], transform_[0][0]);
}

glm::vec2 Transformable2D::GetScale() const {
  return {transform_[0][0], transform_[1][1]};
}


void Transformable2D::TranslateTo(float x, float y) {
  transform_[2] = glm::vec3(x, y, 1.0f);
}
void Transformable2D::TranslateTo(const glm::vec2& pos) {
  transform_[2] = glm::vec3(pos, 1.0f);
}

void Transformable2D::TranslateOn(float x, float y) {
  transform_[2] += glm::vec3(x, y, 1.0f);
}
void Transformable2D::TranslateOn(const glm::vec2& delta) {
  transform_[2] += glm::vec3(delta, 1.0f);
}

void Transformable2D::TranslateOnRelated(
    float len, float origin_x, float origin_y) {
  glm::vec2 direction = GetPosition() - glm::vec2(origin_x, origin_y);
  transform_[2] = glm::vec3(len * direction, 1.0f);
}
void Transformable2D::TranslateOnRelated(
    float len, const glm::vec2& origin_pos) {
  glm::vec2 direction = GetPosition() - origin_pos;
  transform_[2] = glm::vec3(len * direction, 1.0f);
}


void Transformable2D::RotateTo(float radians) {
  glm::vec2 scale = GetScale();
  glm::vec2 pos = GetPosition();

  float radians_cos = glm::cos(radians);
  float radians_sin = glm::sin(radians);

  transform_ = glm::mat3(radians_cos, -radians_sin, 0.0f,
                         radians_sin, radians_cos,  0.0f,
                         0.0f,        0.0f,         1.0f);
  ScaleTo(scale);
  TranslateTo(pos);
}
void Transformable2D::RotateTo(const glm::quat& rot) {
  glm::vec2 scale = GetScale();
  glm::vec2 pos = GetPosition();

  transform_ = glm::mat3_cast(rot);
  ScaleTo(scale);
  TranslateTo(pos);
}

void Transformable2D::RotateOn(float radians) {
  float new_angle = GetRotationRad() + radians;
  RotateTo(new_angle);
}
void Transformable2D::RotateOn(const glm::quat& rot) {
  RotateTo(GetRotationQuat() + rot);
}

void Transformable2D::RotateOnRelated(
    float radians, float origin_x, float origin_y) {
  RotateOn(radians);
  RotateOnRelatedTranslation(radians, {origin_x, origin_y});
}
void Transformable2D::RotateOnRelated(
    float radians, const glm::vec2& origin_pos) {
  RotateOn(radians);
  RotateOnRelatedTranslation(radians, origin_pos);
}
void Transformable2D::RotateOnRelated(
    const glm::quat& rot, float origin_x, float origin_y) {
  RotateOn(rot);
  RotateOnRelatedTranslation(rot, {origin_x, origin_y});
}
void Transformable2D::RotateOnRelated(
    const glm::quat& rot, const glm::vec2& origin_pos) {
  RotateOn(rot);
  RotateOnRelatedTranslation(rot, origin_pos);
}


void Transformable2D::ScaleTo(float factor) {
  transform_[0][0] = factor;
  transform_[1][1] = factor;
}
void Transformable2D::ScaleTo(float x, float y) {
  transform_[0][0] = x;
  transform_[1][1] = y;
}
void Transformable2D::ScaleTo(const glm::vec2& scale) {
  transform_[0][0] = scale.x;
  transform_[1][1] = scale.y;
}

void Transformable2D::ScaleOn(float diff) {
  transform_[0][0] += diff;
  transform_[1][1] += diff;
}
void Transformable2D::ScaleOn(float x, float y) {
  transform_[0][0] += x;
  transform_[1][1] += y;
}
void Transformable2D::ScaleOn(const glm::vec2& scale) {
  transform_[0][0] += scale.x;
  transform_[1][1] += scale.y;
}

void Transformable2D::ScaleOnRelated(
    float factor, float origin_x, float origin_y) {
  TranslateOnRelated(factor, origin_x, origin_y);
  ScaleOn(factor);
}
void Transformable2D::ScaleOnRelated(
    float factor, const glm::vec2& origin_pos) {
  TranslateOnRelated(factor, origin_pos);
  ScaleOn(factor);
}


void Transformable2D::RotateOnRelatedTranslation(
    float radians, const glm::vec2& origin_pos) {
  glm::vec2 direction = glm::abs(GetPosition() - origin_pos);
  float len = glm::length(direction);

  /// cos theorem for our isosceles triangle
  float beta = (glm::pi<float>() - radians) / 2;
  float hypotenuse = 2 * len * len * (1 - glm::cos(radians));

  TranslateTo({origin_pos.x + hypotenuse * glm::sin(beta),
               origin_pos.y + hypotenuse * glm::cos(beta)});
}

void Transformable2D::RotateOnRelatedTranslation(
    const glm::quat& rot, const glm::vec2& origin_pos) {
  glm::vec3 direction {GetPosition() - origin_pos, 0.0f};
  glm::vec3 rotated_direction = glm::mat3_cast(rot) * direction;

  float len = glm::length(rotated_direction);
  float beta = glm::pi<float>() - glm::acos(rotated_direction.x / len);

  float beta_cos = glm::cos(beta);
  float beta_sin = glm::sin(beta);

  float hypotenuse = 2 * len * len * (1 - beta_cos);

  TranslateTo({origin_pos.x + hypotenuse * beta_sin,
               origin_pos.y + hypotenuse * beta_cos});
}

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

/// Animatable

void Animatable::RunAnimation(int animation_id) {
  animation_.SetNext(animation_id);
}

void Animatable::RunAnimationForce(int animation_id) {
  animation_.Run(animation_id);
}

void Animatable::UpdateAnimation(int time) {
  // load interpolated data to cur_pose and then update ubo_
}

void Animatable::StopAnimation() {
  animation_.Stop();
}

/// Collidable

Collidable::Collidable() {} // TODO: DrawManager

void Collidable::EnableCollision() {}
void Collidable::DisableCollision() {}

/// Drawable

Drawable::Drawable() {} // TODO: DrawManager

void Drawable::EnableVisibility() {}
void Drawable::DisableVisibility() {}

/// SoundEmittable

void SoundEmittable::PlaySound(int sound_id) {}

} // namespace faithful
