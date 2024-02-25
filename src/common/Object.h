#ifndef FAITHFUL_OBJECT_H
#define FAITHFUL_OBJECT_H

#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "Animation.h"
#include "../loader/Texture2D.h"
#include "../loader/Sound.h"
#include "../loader/ShaderProgram.h"

#include "../../utils/ConstexprVector.h"

#include "../collision/Aabb.h"

namespace faithful {

namespace details {

class AudioThreadPool;

} // namespace details

class Collision;

class Transformable {
 public:
  Transformable() = delete;
  Transformable(glm::mat3& transform) : transform_(transform) {}

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
  void RotateOnRelatedTranslation(float radians,
                                  const glm::vec2& origin_pos);
  void RotateOnRelatedTranslation(const glm::quat& rot,
                                  const glm::vec2& origin_pos);

  glm::mat3& transform_;
};

class Animatable {
 public:
  Animatable() = default;

  /// will be run after the current animation has finished.
  void RunAnimation(int animation_id);

  /// don't wait until the previous animation has finished; start immediately
  void RunAnimationForce(int animation_id);

  void UpdateAnimation(int time);

  /// clears all set animations
  void StopAnimation();

 protected:
  Animation animation_;

  /// used as intermediate buffer between interpolated data and ubo
  std::unique_ptr<glm::mat4> cur_pose_;

  unsigned int ubo_;
  unsigned int bone_num_ = 0;
  // TODO: shader program for each instance (ubo binding)
};

class Collidable : public Transformable {
 public:
  Collidable() = delete;
  Collidable(glm::mat3& transform, details::Aabb& bounds)
      : Transformable(transform), bounds_(bounds) {}

  const Collision* GetCollision() const {
    return collision_;
  }
  void SetCollition(details::Aabb bounds) {
    bounds_ = bounds;
  }

  glm::vec2 Centroid() {
    return .5f * bounds_.min_ + .5f * bounds_.max_;
  }

 protected:
  details::Aabb& bounds_;
  Collision* collision_;
};

class Drawable {
 public:
  Drawable() = default;

  virtual bool Draw() = 0;

 protected:
  struct Materials {
    Texture2D albedo;
    Texture2D rough_metal;
    Texture2D normal;
    Texture2D occlusion;
    Texture2D emission;
  };

  /**
   * Because of astc encoding format we always have RGBA; so to store
   * rough_metal or normal map we do (official astc-enc recommendations):
   * """
   * To encode this we need to store only two input components
   * in the compressed data, and therefore use the rrrg coding swizzle
   * to align the data with the ASTC luminance+alpha endpoint.
   * """
   * So we decided to represent material as:
   * . albedo - rgba;
   * . metallic & roughness - as rrrg coding swizzle;
   * . normal - as rrrg coding swizzle;
   * - occlusion - rrrr
   * . emission - RGB
   * TODO: occlusion & emission can be stored in the same RGBA texture
   */

  ShaderProgram shader_program_;
  Materials materials_;

  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int ebo_ = 0;
  unsigned int ebo_idx_num_ = 0;
};

class SoundEmittable {
 public:
  SoundEmittable() = default;

  void PlaySound(int sound_id);

  void SetAudioThreadPool(details::AudioThreadPool* audio_thread_pool) {
    audio_thread_pool_ = audio_thread_pool;
  }

 protected:
  details::AudioThreadPool* audio_thread_pool_ = nullptr;
  utils::ConstexprVector<Sound, 5> sounds_;
};

/// object can be updatable but without animations
class Updatable {
 public:
  virtual void Update() = 0;
};

}  // namespace faithful

#endif  // FAITHFUL_OBJECT_H
