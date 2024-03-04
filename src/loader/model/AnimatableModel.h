#ifndef FAITHFUL_SRC_LOADER_MODEL_ANIMATABLEMODEL_H_
#define FAITHFUL_SRC_LOADER_MODEL_ANIMATABLEMODEL_H_

#include <map>
#include <vector>

#include <tiny_gltf.h>

#include "ModelBase.h"
#include "../ShaderProgram.h"

#include "../../common/TransformableObject.h"

// TODO: should Update cur_pose_

namespace faithful {

class AnimatableModel
    : public ModelBase,
      public TransformableObject {
 public:
  using Base = ModelBase;
  using Base::Base;
  using Base::operator=;

  void Draw(GLuint ubo, ShaderProgram& shader_program);

  void UpdateAnimation(float time, float framerate);

  void RunAnimation(int animation_id);
  void StopAnimation();

  void SetCurrentAnimation(int animation_id);
  void UnsetCurrentAnimation();

  void SetNextAnimation(int animation_id);
  void UnsetNextAnimation();
  void ResetTimeAnimation();

 private:
  void DrawMesh(tinygltf::Mesh &mesh);
  void DrawModelNodes(tinygltf::Node &node);

  std::vector<glm::mat4> cur_pose_{0};

  int cur_animation_id_ = -1;
  int next_animation_id_ = -1;

  double time_ = 0;
  bool repeat_ = false;

  using Base::data_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_ANIMATABLEMODEL_H_
