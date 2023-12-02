#ifndef FAITHFUL_OBJECT_H
#define FAITHFUL_OBJECT_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <utility>
#include <forward_list>
#include <vector>
#include <map>

#include <assimp/scene.h>

#include "../../utils/Span.h"
#include "Mesh.h"
#include "common/RunningAnimation.h"
#include "common/AnimationNode.h"

namespace faithful {

namespace utility {
class ShaderProgram;
} // namespace utility

class Texture;
class Sprite;

// TODO (maybe one day...) : NO origin (only functions: rotate/scale RelativeToOrigin)

// TODO: ctor-s for initially "not drawable"
// TODO: object 2d skinned/ not skinned, plural / not plural

// TODO: getters/setters/ rotateOn, rotateTo, scale, translate..... 2d/3d

/// Object, Object2D, Object3D and their Skinned versions
/// occupy total 32 bytes

/**
 * by default all objects are drawable and collisionable
 * */
class Object {
 public:
  Object() = default;

  /// no setters for id
  unsigned int get_global_id() const {
    return global_id_;
  }
  unsigned int get_local_id() const {
    return global_id_;
  }

  unsigned int get_texture_id() const {
    return global_id_;
  }

  bool Drawable() const {
    // TODO:
  }
  bool Collisionable() const {
    // TODO:
  }

  const Object* get_collision() const {
    return collision_;
  }
  void set_collision(const Object& collision) {
    *collision_ = collision;
  }

  unsigned int get_texture() const {
    return texture_id_;
  }
  void set_texture(const Sprite&) {}
  void set_texture(const Texture&) {}
  void set_texture(unsigned int image_id) {}

  void DetectCollision() const;
  void MakeVisible();
  void MakeUnvisible();
  void MakeCollisionable();
  void MakeUncollisionable();

  /// How it should be used: inherit from Object and
  ///   simply HIDE it by redeclaring (we don't need V-tables)
  void RunAnimation(int animation_id) {}

 protected:
  Object* collision_ = nullptr;
  unsigned int global_id_;
  unsigned int local_id_; // + {drawable, collisionable} flags
  unsigned int texture_id_;
};

class Object2D : public Object {
 public:
  Object2D() = default;
  // 0 - the highest, 1 - lower, -1 - the lowest
  void set_draw_priority(int draw_priority) {
    draw_priority_ = draw_priority;
  }
  int get_draw_priority() {
    return draw_priority_;
  }

  // TODO: does the OpenMP has sense
  glm::vec2 get_position() const {}
  float get_rotation() const {}
  glm::vec2 get_scale() const {}

  void TranslateTo(float x, float y) {}
  void TranslateOn(float x, float y) {}

  void RotateTo(float angle) {}
  void RotateOn(float angle) {}
  void RotateToRelated(float angle, float origin_x, float origin_y) {}
  void RotateOnRelated(float angle, float origin_x, float origin_y) {}

  void ScaleTo(float x, float y) {}
  void ScaleOn(float x, float y) {}
  void ScaleToRelated(float x, float y, float origin_x, float origin_y) {}
  void ScaleOnRelated(float x, float y, float origin_x, float origin_y) {}

  /// Some syntactic sugar for convenience
  void TranslateTo(glm::vec2 position) {}
  void TranslateOn(glm::vec2 position) {}

  void RotateToRelated(float angle, glm::vec2 origin_position) {}
  void RotateOnRelated(float angle, glm::vec2 origin_position) {}

  void ScaleTo(glm::vec2 size) {}
  void ScaleOn(glm::vec2 size) {}
  void ScaleToRelated(glm::vec2 size, glm::vec2 origin_position) {}
  void ScaleOnRelated(glm::vec2 size, glm::vec2 origin_position) {}

 protected:
  int draw_priority_ = -1;
  glm::mat3* transform_ = nullptr;
};

class Object3D : public Object {
 public:
  Object3D() = default;

  // TODO: does the OpenMP has sense

  glm::vec3 get_position() const {
    return {(*transform_)[3].x, (*transform_)[3].y, (*transform_)[3].z};
  }
  glm::vec3 get_rotation() const {}
  glm::vec3 get_scale() const {}

  void TranslateTo(float x, float y, float z) {
    (*transform_)[3].x = x;
    (*transform_)[3].y = y;
    (*transform_)[3].z = z;
  }
  void TranslateOn(float x, float y, float z) {
    *transform_ = glm::translate(*transform_, glm::vec3(x, y, z));
  }

  void RotateTo(float angle, float x, float y, float z) {}
  void RotateOn(float angle, float x, float y, float z) {
    *transform_ = glm::rotate(*transform_, glm::radians(angle), glm::vec3(x, y, z));
  }
  void RotateToRelated(float angle, float x, float y, float z,
                       float origin_x, float origin_y, float origin_z) {}
  void RotateOnRelated(float angle, float x, float y, float z,
                       float origin_x, float origin_y, float origin_z) {}

  void ScaleTo(float x, float y, float z) {}
  void ScaleOn(float x, float y, float z) {
    *transform_ = glm::scale(*transform_, glm::vec3(x, y, z));
  }
  void ScaleToRelated(float x, float y, float z,
                      float origin_x, float origin_y, float origin_z) {}
  void ScaleOnRelated(float x, float y, float z,
                      float origin_x, float origin_y, float origin_z) {}

  /// Some syntactic sugar for convenience
  void TranslateTo(glm::vec3 position) {
    TranslateTo(position.x, position.y, position.z);
  }
  void TranslateOn(glm::vec3 position) {
    TranslateOn(position.x, position.y, position.z);
  }

  void RotateTo(float angle, glm::vec3 rotation) {}
  void RotateOn(float angle, glm::vec3 rotation) {}
  void RotateToRelated(float angle, glm::vec3 rotation,
                       glm::vec3 origin_position) {}
  void RotateOnRelated(float angle, glm::vec3 rotation,
                       glm::vec3 origin_position) {}

  void ScaleTo(glm::vec3 size) {}
  void ScaleOn(glm::vec3 size) {}
  void ScaleToRelated(glm::vec3 size, glm::vec3 origin_position) {}
  void ScaleOnRelated(glm::vec3 size, glm::vec3 origin_position) {}

 protected:
  glm::mat4* transform_ = nullptr;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////       IMPLEMENTATION       ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class ObjectRenderCategory {
  kDefault,
  kHeightMap,
  kHeightParallaxMap
};

enum class ObjectRenderPhase {
  kGeometry,
  kLight
};

class ObjectImplBase {
 public:
  ObjectImplBase() = default;
  unsigned int get_global_id() const {
    return global_id_;
  }
  unsigned int get_local_id() const {
    return last_local_id_;
  }
  unsigned int get_vao() const {
    return vao_;
  }
  unsigned int get_vbo() const {
    return vbo_;
  }
  unsigned int get_ebo() const {
    return ebo_;
  }
 protected:
  static unsigned int NewGlobalId() {
static int skinless_mesh_size;
static int skinned_mesh_size;
static int metadata_size;
    return ++last_global_id_;
  }

  unsigned int last_local_id_ = 0;
  unsigned int global_id_ = 0;
  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int ebo_ = 0;
  bool ready_instantiate_ = false;

 private:
  static unsigned int last_global_id_;
};

class SingleShaderObjectImplBase : public ObjectImplBase {
 public:
  SingleShaderObjectImplBase() = default;

 protected:
  mutable utility::ShaderProgram *shader_program_ = nullptr;
};

class TrivialObjectImplBase : public SingleShaderObjectImplBase {
 public:
  TrivialObjectImplBase() = default;
  // TODO: depends on _static_load_ bool
  void Configurate1(unsigned int ebo_idx_num);

 protected:
  /// default texture is DefaultSprites::ids_[1]
  unsigned int ebo_idx_num_ = 0;
};

/// Cube, Sphere, Cone, etc.
/// "Trivial" means object with only 1 Mesh
class TrivialObject2DImpl : public TrivialObjectImplBase {
 public:
  TrivialObject2DImpl();

  std::tuple<glm::mat3*, int, int> CreateInstance();
  void Draw(ObjectRenderPhase phase) const;

  void MakeDrawable(unsigned int local_id);
  void MakeUndrawable(unsigned int local_id);

  glm::mat3* get_transform(unsigned int local_id) const;

 protected:
  struct InstanceInfo {
    glm::mat3* transform;
    unsigned int texture_id;
  };
  std::map<unsigned int, InstanceInfo> drawable_instances_;
  std::map<unsigned int, InstanceInfo> undrawable_instances_;
};

/// for models with single Mesh
class TrivialObject3DImpl : public TrivialObjectImplBase {
 public:
  TrivialObject3DImpl();

  std::tuple<glm::mat4*, int, int> CreateInstance();
  void Draw(ObjectRenderPhase phase) const;

  void MakeDrawable(unsigned int local_id);
  void MakeUndrawable(unsigned int local_id);

  glm::mat4* get_transform(unsigned int local_id) const;

 protected:
  struct InstanceInfo {
    glm::mat4* transform;
    Material material;
  };
  std::map<unsigned int, InstanceInfo> drawable_instances_;
  std::map<unsigned int, InstanceInfo> undrawable_instances_;
};

class MultimeshObject3DImpl : public SingleShaderObjectImplBase {
 public:
  MultimeshObject3DImpl() = default;

  // TODO: depends on _static_load_ bool
  void Configurate1(unsigned int num);
  void Configurate2(utility::Span<Mesh*> meshes,
                    ObjectRenderCategory category);

  std::tuple<glm::mat4*, int, int> CreateInstance();
  void Draw(ObjectRenderPhase phase) const;


  void MakeDrawable(unsigned int local_id);
  void MakeUndrawable(unsigned int local_id);

  glm::mat4* get_transform(unsigned int local_id) const;

 protected:
  std::map<unsigned int, glm::mat4*> drawable_instances_transform_;
  std::map<unsigned int, glm::mat4*> undrawable_instances_transform_;
  mutable utility::Span<Mesh*> meshes_; // TODO: NOT-mutable
};

class SkinnedObject3DImpl : public ObjectImplBase {
 public:
  SkinnedObject3DImpl() = default;

  // TODO: depends on _static_load_ bool
  void Configurate1(unsigned int num);
  void Configurate2(utility::Span<Mesh*> meshes,
                    ObjectRenderCategory category,
                    unsigned int bone_num,
                    utility::Span<AnimationNode*> animation_nodes,
                    glm::mat4 global_inverse_transform);

  std::tuple<glm::mat4*, int, int> CreateInstance();
  void Draw(ObjectRenderPhase phase) const;
  void DefaultPose(unsigned int ubo);

  void MakeDrawable(unsigned int local_id);
  void MakeUndrawable(unsigned int local_id);

  glm::mat4* get_transform(unsigned int local_id) const;

  // TODO: not Run() but SetPose() ------------< ? IDK what this means

  void RunAnimation(unsigned int local_id, int anim_id, bool repeat = false);
  void Update(double framerate);

 protected:
  friend class RunningAnimation;
  //////////////////////////////////////   CAUTION   /////////////////////////////////////////
  // TODO: for each phase you should add new data into vector !!!!!!<-----------------------
  struct InstanceInfo {
    glm::mat4* transform;
    utility::ShaderProgram* shader_program;
    unsigned int ubo;
  };

  std::map<unsigned int, InstanceInfo> drawable_instances_;
  std::map<unsigned int, InstanceInfo> undrawable_instances_;

  mutable utility::Span<Mesh*> meshes_;
  utility::Span<AnimationNode*> animation_nodes_;
  glm::mat4 global_inverse_transform_ = glm::mat4(1.0f); // TODO: not necessary (?)
  unsigned int bone_num_ = 0;

  std::forward_list<RunningAnimation> animated_objects_;

  //  TODO: cur_pose can't be parallelized (make few copies?)
  /// Used as buffer before loading to UBO
  mutable utility::Span<glm::mat4> cur_pose_;

 private:
  static unsigned int last_ubo_bind_point_;
};


/// --------------------------------
/// --------NOT_IMPLEMENTED_________
/// ________________________________
class MultimeshObject2DImpl : TrivialObject2DImpl {};
class SkinnedObject2DImpl {};
class SkinlessAnimatedObject2DImpl {
  // rather rapid sprite switching
};

} // namespace faithful

#endif // FAITHFUL_OBJECT_H
