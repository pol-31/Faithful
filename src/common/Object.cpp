#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../executors/AudioThreadPool.h"

namespace faithful {

/// Transformable

glm::vec2 Transformable::GetPosition() const {
  return glm::vec2(transform_[2]);
}

glm::quat Transformable::GetRotationQuat() const {
  return glm::quat_cast(transform_);
}
float Transformable::GetRotationRad() const {
  return std::atan2(transform_[1][0], transform_[0][0]);
}

glm::vec2 Transformable::GetScale() const {
  return {transform_[0][0], transform_[1][1]};
}


void Transformable::TranslateTo(float x, float y) {
  transform_[2] = glm::vec3(x, y, 1.0f);
}
void Transformable::TranslateTo(const glm::vec2& pos) {
  transform_[2] = glm::vec3(pos, 1.0f);
}

void Transformable::TranslateOn(float x, float y) {
  transform_[2] += glm::vec3(x, y, 1.0f);
}
void Transformable::TranslateOn(const glm::vec2& delta) {
  transform_[2] += glm::vec3(delta, 1.0f);
}

void Transformable::TranslateOnRelated(
    float len, float origin_x, float origin_y) {
  glm::vec2 direction = GetPosition() - glm::vec2(origin_x, origin_y);
  transform_[2] = glm::vec3(len * direction, 1.0f);
}
void Transformable::TranslateOnRelated(
    float len, const glm::vec2& origin_pos) {
  glm::vec2 direction = GetPosition() - origin_pos;
  transform_[2] = glm::vec3(len * direction, 1.0f);
}


void Transformable::RotateTo(float radians) {
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
void Transformable::RotateTo(const glm::quat& rot) {
  glm::vec2 scale = GetScale();
  glm::vec2 pos = GetPosition();

  transform_ = glm::mat3_cast(rot);
  ScaleTo(scale);
  TranslateTo(pos);
}

void Transformable::RotateOn(float radians) {
  float new_angle = GetRotationRad() + radians;
  RotateTo(new_angle);
}
void Transformable::RotateOn(const glm::quat& rot) {
  RotateTo(GetRotationQuat() + rot);
}

void Transformable::RotateOnRelated(
    float radians, float origin_x, float origin_y) {
  RotateOn(radians);
  RotateOnRelatedTranslation(radians, {origin_x, origin_y});
}
void Transformable::RotateOnRelated(
    float radians, const glm::vec2& origin_pos) {
  RotateOn(radians);
  RotateOnRelatedTranslation(radians, origin_pos);
}
void Transformable::RotateOnRelated(
    const glm::quat& rot, float origin_x, float origin_y) {
  RotateOn(rot);
  RotateOnRelatedTranslation(rot, {origin_x, origin_y});
}
void Transformable::RotateOnRelated(
    const glm::quat& rot, const glm::vec2& origin_pos) {
  RotateOn(rot);
  RotateOnRelatedTranslation(rot, origin_pos);
}


void Transformable::ScaleTo(float factor) {
  transform_[0][0] = factor;
  transform_[1][1] = factor;
}
void Transformable::ScaleTo(float x, float y) {
  transform_[0][0] = x;
  transform_[1][1] = y;
}
void Transformable::ScaleTo(const glm::vec2& scale) {
  transform_[0][0] = scale.x;
  transform_[1][1] = scale.y;
}

void Transformable::ScaleOn(float diff) {
  transform_[0][0] += diff;
  transform_[1][1] += diff;
}
void Transformable::ScaleOn(float x, float y) {
  transform_[0][0] += x;
  transform_[1][1] += y;
}
void Transformable::ScaleOn(const glm::vec2& scale) {
  transform_[0][0] += scale.x;
  transform_[1][1] += scale.y;
}

void Transformable::ScaleOnRelated(
    float factor, float origin_x, float origin_y) {
  TranslateOnRelated(factor, origin_x, origin_y);
  ScaleOn(factor);
}
void Transformable::ScaleOnRelated(
    float factor, const glm::vec2& origin_pos) {
  TranslateOnRelated(factor, origin_pos);
  ScaleOn(factor);
}


void Transformable::RotateOnRelatedTranslation(
    float radians, const glm::vec2& origin_pos) {
  glm::vec2 direction = glm::abs(GetPosition() - origin_pos);
  float len = glm::length(direction);

  /// cos theorem for our isosceles triangle
  float beta = (glm::pi<float>() - radians) / 2;
  float hypotenuse = 2 * len * len * (1 - glm::cos(radians));

  TranslateTo({origin_pos.x + hypotenuse * glm::sin(beta),
               origin_pos.y + hypotenuse * glm::cos(beta)});
}

void Transformable::RotateOnRelatedTranslation(
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

/// SoundEmittable

void SoundEmittable::PlaySound(int sound_id) {
  audio_thread_pool_->Play(sounds_[sound_id]);
}

} // namespace faithful
