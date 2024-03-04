#ifndef FAITHFUL_SRC_COMMON_TRANSFORMABLEOBJECT_H_
#define FAITHFUL_SRC_COMMON_TRANSFORMABLEOBJECT_H_

#include <glm/glm.hpp>

namespace faithful {

class TransformableObject {
 public:
  TransformableObject() = delete;

  glm::vec2 GetPosition() const;

  glm::quat GetRotationQuat() const;
  float GetRotationRad() const;

  glm::vec2 GetScale() const;

  /// positioning
  void TranslateTo(float x, float y);
  void TranslateTo(const glm::vec2& pos);

  void TranslateOn(float x, float y);
  void TranslateOn(const glm::vec2& delta);

  void TranslateOnRelated(float len, float origin_x, float origin_y);
  void TranslateOnRelated(float len, const glm::vec2& origin_pos);

  /// rotation
  void RotateTo(float radians);
  void RotateTo(const glm::quat& rot);

  void RotateOn(float radians);
  void RotateOn(const glm::quat& rot);

  void RotateOnRelated(float radians, float origin_x, float origin_y);
  void RotateOnRelated(float radians, const glm::vec2& origin_pos);
  void RotateOnRelated(const glm::quat& rot, float origin_x, float origin_y);
  void RotateOnRelated(const glm::quat& rot, const glm::vec2& origin_pos);

  /// scale
  void ScaleTo(float factor);
  void ScaleTo(float x, float y);
  void ScaleTo(const glm::vec2& scale);

  void ScaleOn(float diff);
  void ScaleOn(float x, float y);
  void ScaleOn(const glm::vec2& scale);

  void ScaleOnRelated(float factor, float origin_x, float origin_y);
  void ScaleOnRelated(float factor, const glm::vec2& origin_pos);

 protected:
  glm::mat3 transform_; // TODO: only in 2D space ?

 private:
  void RotateOnRelatedTranslation(float radians,
                                  const glm::vec2& origin_pos);
  void RotateOnRelatedTranslation(const glm::quat& rot,
                                  const glm::vec2& origin_pos);
};

} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_TRANSFORMABLEOBJECT_H_
