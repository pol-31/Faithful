#ifndef FAITHFUL_OBJECT_H
#define FAITHFUL_OBJECT_H

#include <array>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../utils/Span.h"

#include "../loader/Shader.h"
#include "../loader/Texture2D.h"
#include "../common/Animation.h"
#include "../loader/Sound.h"

#include "../collision/Collision.h"

#include "../Engine.h"


namespace faithful {

/** What OBJECTS do we have:
 * - Camera; POSITION (neither drawable nor collisionable)
 * - Cursor; POSITION, DRAWABLE
 * -
 * - Models; POSITION, DRAWABLE, ANIMATION, MATERIAL; (enemy, character, obstacles) - external/internal;
 * - Terrain; POSITION, DRAWABLE, ANIMATION, MATERIAL
 * - Sky, sky-entities; POSITION, DRAWABLE, ANIMATION, MATERIAL
 * - procedural grass/water, pacticle effects; POSITION, DRAWABLE, ANIMATION, MATERIAL
 * - GUI;  POSITION, DRAWABLE, ANIMATION, MATERIAL; (text fields, button, slider, checkbox, etc...);
 * // + sprite/image (maybe as part of a GUI);
 *
 * 2D: Cursor, gui
 * 3D: camera, models, terrain, sky, procedural_gen
 *
 * --- --- --- --- --- --- --- --- unified methods (don't need vtables)
 * UNIFIED INTERFACE:
 * POSITION (2D/3D): transform matrix                       --> mat3/mat4
 * COLLISIONABLE: COLLIDE_TEST, COLLIDER:ON/OFF     --> __Collision*__
 * --- --- --- --- --- --- --- --- virtual methods (+vtables)
 * DIFFERENT INTERFACES:
 * DRAWABLE: LOAD (from memory), DRAW, VISIBLE:ON/OFF
 *   --> virtual because of convenient Load() calling
*    + (MATERIAL: is it just ids of opengl_textures (?)  --> std::array<Material, num> OR std::vector)
 * ANIMATED: ANIMATION:RUN/STOP
 *   --> virtual GetFrame(time) + unified Time/RunningAnimation interface
 * --- --- --- --- --- --- --- ---
 * */

// TODO: global_id, local_id, last_global_id_, last_local_id_

class Transformable2D {
 public:
  Transformable2D() = default;

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

  glm::mat3 transform_;
};

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

 private:
  Animation animation_;

  /// used as intermediate buffer between interpolated data and ubo
  utility::Span<glm::mat4> cur_pose_;

  unsigned int ubo_;
  unsigned int bone_num_ = 0;
};

class Collidable {
 public:
  Collidable();

  void EnableCollision();
  void DisableCollision();

  const Collision* GetCollision() const {
    return collision_;
  }

 private:
  Collision* collision_;
};

// Drawable (always materialized)
class Drawable {
 public:
  Drawable();

  virtual bool Load() = 0;
  virtual bool Draw() = 0;

  void EnableVisibility();
  void DisableVisibility();

  /// 0 - the highest, 1-2-... - lower, -1 - the lowest
  /// for -1 then we can just cast to it uint and get uint max
  void SetDrawPriority(int draw_priority) {
    draw_priority_ = draw_priority;
  }
  int GetDrawPriority() const {
    return draw_priority_;
  }

 private:
  struct Materials {
    Texture2D albedo;
    Texture2D rough_metal;
    Texture2D normal;
    Texture2D height;
    Texture2D ao;
    // emmision ?
  };

  ShaderProgram* shader_program;
  Materials materials_;

  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int ebo_ = 0;
  unsigned int ebo_idx_num_ = 0;

  int draw_priority_ = -1;
};

class SoundEmittable {
 public:
  SoundEmittable() = default;

  void PlaySound(int sound_id);

 private:
  std::vector<Sound> sounds; // TODO: need some kind of std::array-like vector
};

class Updatable {
 public:
  virtual void Update() = 0;
};

}  // namespace faithful

#endif  // FAITHFUL_OBJECT_H
