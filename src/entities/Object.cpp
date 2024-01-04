#include "Object.h"

#include <glm/gtc/type_ptr.hpp>
#include <utility>

#include "Mesh.h"
#include "common/RunningAnimation.h"
#include "../loader/ShaderProgram.h"
#include "../loader/Sprite.h"
#include "../loader/Texture.h"
#include "../Engine.h"

namespace faithful {

void TrivialObjectImplBase::Configurate1(unsigned int ebo_idx_num) {
  global_id_ = NewGlobalId();
  ebo_idx_num_ = ebo_idx_num;
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  ready_instantiate_ = true;
}

TrivialObject2DImpl::TrivialObject2DImpl() {
  shader_program_ = utility::DefaultShaderProgram::object2d_;
}

std::tuple<glm::mat3*, int, int> TrivialObject2DImpl::CreateInstance() {
  assert(ready_instantiate_);
  auto new_instance_transform = new glm::mat3(1.0f);
  drawable_instances_.insert(
    {++last_local_id_, {new_instance_transform, DefaultSprites::ids_[1]}});
  return {new_instance_transform, global_id_, last_local_id_};
}

void TrivialObject2DImpl::Draw(ObjectRenderPhase phase) const {
  // TODO: draw regard to phase
  shader_program_->Use();
  shader_program_->AdjustVar("texture1", 0);
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);

  for (auto inst : drawable_instances_) {
    glBindTexture(GL_TEXTURE_2D, inst.second.texture_id);
    shader_program_->AdjustMat3v("transform", 1, GL_FALSE,
                                 glm::value_ptr(*inst.second.transform));
    glDrawArrays(GL_TRIANGLE_FAN, 0, ebo_idx_num_);
  }
}

void TrivialObject2DImpl::MakeDrawable(unsigned int local_id) {
  drawable_instances_.insert(
    std::move(undrawable_instances_.extract(local_id)));
}

void TrivialObject2DImpl::MakeUndrawable(unsigned int local_id) {
  undrawable_instances_.insert(
    std::move(drawable_instances_.extract(local_id)));
}

glm::mat3* TrivialObject2DImpl::get_transform(unsigned int local_id) const {
  auto found_transform = drawable_instances_.find(local_id);
  if (found_transform != drawable_instances_.end())
    return found_transform->second.transform;

  found_transform = undrawable_instances_.find(local_id);
  if (found_transform != undrawable_instances_.end())
    return found_transform->second.transform;

  return nullptr;
}

TrivialObject3DImpl::TrivialObject3DImpl() {
  shader_program_ = utility::DefaultShaderProgram::object3d_;
}


////////////////////////////______MATERIAL_______/////////////////////////
////////////////////////////______MATERIAL_______/////////////////////////
////////////////////////////______MATERIAL_______/////////////////////////
////////////////////////////______MATERIAL_______/////////////////////////
std::tuple<glm::mat4*, int, int> TrivialObject3DImpl::CreateInstance() {
  assert(ready_instantiate_);
  auto new_instance_transform = new glm::mat4(1.0f);
  drawable_instances_.insert( // TODO: NOT tex_id, BUT material
    {++last_local_id_, {new_instance_transform, DefaultTextures::ids_[4]}});
  return {new_instance_transform, global_id_, last_local_id_};
}

void TrivialObject3DImpl::Draw(ObjectRenderPhase phase) const {
  // TODO: draw regard to phase
  shader_program_->Use();
  glBindVertexArray(vao_);

  for (auto inst : drawable_instances_) {
    glActiveTexture(GL_TEXTURE0);
    shader_program_->AdjustVar("texture_albedo", 0);
    glBindTexture(GL_TEXTURE_2D, inst.second.material.tex_albedo_);
    glActiveTexture(GL_TEXTURE1);
    shader_program_->AdjustVar("texture_roughness", 1);
    glBindTexture(GL_TEXTURE_2D, inst.second.material.tex_roughness_);
    glActiveTexture(GL_TEXTURE2);
    shader_program_->AdjustVar("texture_metallic", 2);
    glBindTexture(GL_TEXTURE_2D, inst.second.material.tex_metallic_);
    glActiveTexture(GL_TEXTURE3);
    shader_program_->AdjustVar("texture_normal", 3);
    glBindTexture(GL_TEXTURE_2D, inst.second.material.tex_normal_);
    glActiveTexture(GL_TEXTURE4);
    shader_program_->AdjustVar("texture_height", 4);
    glBindTexture(GL_TEXTURE_2D, inst.second.material.tex_height_);

    shader_program_->AdjustMat4v("transform", 1, GL_FALSE,
                                 glm::value_ptr(*inst.second.transform));

    glDrawElements(GL_TRIANGLES, ebo_idx_num_, GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
}

void TrivialObject3DImpl::MakeDrawable(unsigned int local_id) {
  drawable_instances_.insert(
    std::move(undrawable_instances_.extract(local_id)));
}
void TrivialObject3DImpl::MakeUndrawable(unsigned int local_id) {
  undrawable_instances_.insert(
    std::move(drawable_instances_.extract(local_id)));
}

glm::mat4* TrivialObject3DImpl::get_transform(unsigned int local_id) const {
  auto found_transform = drawable_instances_.find(local_id);
  if (found_transform != drawable_instances_.end())
    return found_transform->second.transform;

  found_transform = undrawable_instances_.find(local_id);
  if (found_transform != undrawable_instances_.end())
    return found_transform->second.transform;

  return nullptr;
}

void MultimeshObject3DImpl::Configurate1(unsigned int num) {
  global_id_ = NewGlobalId();
  GLuint buffer_ids[num];
  glGenVertexArrays(num, buffer_ids);
  vao_ = buffer_ids[0];
  glGenBuffers(num, buffer_ids);
  vbo_ = buffer_ids[0];
  glGenBuffers(num, buffer_ids);
  ebo_ = buffer_ids[0];
}

void MultimeshObject3DImpl::Configurate2(utility::Span<Mesh*> meshes,
                                         ObjectRenderCategory category) {
  meshes_ = std::move(meshes);
  switch (category) {
    case ObjectRenderCategory::kDefault:
      shader_program_ = utility::DefaultShaderProgram::object3d_;
      break;
    case ObjectRenderCategory::kHeightMap:
      shader_program_ = utility::DefaultShaderProgram::object3d_light_;
      break;
    case ObjectRenderCategory::kHeightParallaxMap:
      shader_program_ = utility::DefaultShaderProgram::object3d_light_parallax_;
      break;
  }
  ready_instantiate_ = true;
}

std::tuple<glm::mat4*, int, int> MultimeshObject3DImpl::CreateInstance() {
  assert(ready_instantiate_);
  auto new_instance_transform = new glm::mat4(1.0f);
  drawable_instances_transform_.insert({++last_local_id_, new_instance_transform});
  return {new_instance_transform, global_id_, last_local_id_};
}

void MultimeshObject3DImpl::Draw(ObjectRenderPhase phase) const {
  // TODO: draw regard to phase
  shader_program_->Use();
  for (auto transform : drawable_instances_transform_) {
    for (unsigned int i = 0; i < meshes_.get_size(); ++i) {
      auto cur_mesh = meshes_[i];
      auto cur_index_num = cur_mesh->get_index_num();

      // TODO: wait until Model is ready (check mesh::ebo_num)
      if (cur_index_num == 0) return;

      auto cur_mat = cur_mesh->get_material();
      glActiveTexture(GL_TEXTURE0);
      shader_program_->AdjustVar("texture_albedo", 0);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_albedo_);
      glActiveTexture(GL_TEXTURE1);
      shader_program_->AdjustVar("texture_roughness", 1);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_roughness_);
      glActiveTexture(GL_TEXTURE2);
      shader_program_->AdjustVar("texture_metallic", 2);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_metallic_);
      glActiveTexture(GL_TEXTURE3);
      shader_program_->AdjustVar("texture_normal", 3);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_normal_);
      glActiveTexture(GL_TEXTURE4);
      shader_program_->AdjustVar("texture_height", 4);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_height_);

      shader_program_->AdjustMat4v("model", 1, GL_FALSE,
                                   glm::value_ptr(*(transform.second)));

      glBindVertexArray(vao_ + i);
      glDrawElements(GL_TRIANGLES, cur_index_num, GL_UNSIGNED_INT, 0);
    }
  }
}

void MultimeshObject3DImpl::MakeDrawable(unsigned int local_id) {
  drawable_instances_transform_.insert(
    std::move(undrawable_instances_transform_.extract(local_id)));
}
void MultimeshObject3DImpl::MakeUndrawable(unsigned int local_id) {
  undrawable_instances_transform_.insert(
    std::move(drawable_instances_transform_.extract(local_id)));
}

glm::mat4* MultimeshObject3DImpl::get_transform(
    unsigned int local_id) const {
  auto found_transform = drawable_instances_transform_.find(local_id);
  if (found_transform != drawable_instances_transform_.end())
    return found_transform->second;

  found_transform = undrawable_instances_transform_.find(local_id);
  if (found_transform != undrawable_instances_transform_.end())
    return found_transform->second;

  return nullptr;
}

// the same implementation as in MultimeshObject3DImpl::Configurate1
void SkinnedObject3DImpl::Configurate1(unsigned int num) {
  global_id_ = NewGlobalId();
  // TODO: ubo related to shader_program;
  GLuint buffer_ids[num];
  glGenVertexArrays(num, buffer_ids);
  vao_ = buffer_ids[0];
  glGenBuffers(num, buffer_ids);
  vbo_ = buffer_ids[0];
  glGenBuffers(num, buffer_ids);
  ebo_ = buffer_ids[0];
}

/// intentionally hides TODO: what?
void SkinnedObject3DImpl::Configurate2(utility::Span<Mesh*> meshes,
                  ObjectRenderCategory category,
                  unsigned int bone_num,
                  utility::Span<AnimationNode*> animation_nodes,
                  glm::mat4 global_inverse_transform) {
  bone_num_ = bone_num;
  meshes_ = std::move(meshes);
  // TODO:
  /*switch (category) {
    case ObjectRenderCategory::kDefault:
      shader_program_ = utility::DefaultShaderProgram::object3d_skinned_;
      break;
    case ObjectRenderCategory::kHeightMap:
      shader_program_ = utility::DefaultShaderProgram::object3d_skinned_light_;
      break;
    case ObjectRenderCategory::kHeightParallaxMap:
      shader_program_ = utility::DefaultShaderProgram::object3d_skinned_light_parallax_;
      break;
  }*/
  animation_nodes_ = std::move(animation_nodes);
  global_inverse_transform_ = global_inverse_transform;
  ready_instantiate_ = true;
}

std::tuple<glm::mat4*, int, int> SkinnedObject3DImpl::CreateInstance() {
  assert(ready_instantiate_);
  unsigned int new_ubo;
  glGenBuffers(1, &new_ubo);
  auto new_shader_program = utility::DefaultShaderProgram::CreateBoneProgram();
  auto new_instance_transform = new glm::mat4(1.0f);
  drawable_instances_.insert(
    {++last_local_id_,
     {new_instance_transform, new_shader_program, new_ubo}
    });

  glBindBuffer(GL_UNIFORM_BUFFER, new_ubo);
  glBufferData(GL_UNIFORM_BUFFER, 200 * sizeof(glm::mat4),
               nullptr, GL_DYNAMIC_DRAW);

  GLuint bindingPoint = ++last_ubo_bind_point_;
  glUniformBlockBinding(
    new_shader_program->Id(),
    glGetUniformBlockIndex(new_shader_program->Id(), "BoneData"),
    bindingPoint);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, new_ubo);

  // TODO: EITHER related to instance OR to class (then there's no parallelizing)
  cur_pose_ = utility::Span<glm::mat4>(bone_num_);

  DefaultPose(new_ubo);

  return {new_instance_transform, global_id_, last_local_id_};
}

void SkinnedObject3DImpl::Draw(ObjectRenderPhase phase) const {
  // TODO: draw regard to phase
  for (auto inst : drawable_instances_) {
    auto cur_shader_program = inst.second.shader_program;
    cur_shader_program->Use();
    for (unsigned int i = 0; i < meshes_.get_size(); ++i) {
      auto cur_mesh = meshes_[i];

      // TODO: wait until Model is ready (check mesh::ebo_num)
      if (cur_mesh->get_index_num() == 0) return;

      auto cur_index_num = cur_mesh->get_index_num();
      auto cur_mat = cur_mesh->get_material();
      glActiveTexture(GL_TEXTURE0);
      cur_shader_program->AdjustVar("texture_albedo", 0);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_albedo_);
      glActiveTexture(GL_TEXTURE1);
      cur_shader_program->AdjustVar("texture_roughness", 1);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_roughness_);
      glActiveTexture(GL_TEXTURE2);
      cur_shader_program->AdjustVar("texture_metallic", 2);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_metallic_);
      glActiveTexture(GL_TEXTURE3);
      cur_shader_program->AdjustVar("texture_normal", 3);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_normal_);
      glActiveTexture(GL_TEXTURE4);
      cur_shader_program->AdjustVar("texture_height", 4);
      glBindTexture(GL_TEXTURE_2D, cur_mat.tex_height_);

      cur_shader_program->AdjustMat4v("model", 1, GL_FALSE,
                                      glm::value_ptr(*(inst.second.transform)));

      glBindVertexArray(vao_ + i);
      glDrawElements(GL_TRIANGLES, cur_index_num, GL_UNSIGNED_INT, 0);
    }
  }
}

void SkinnedObject3DImpl::DefaultPose(unsigned int ubo) {
  animation_nodes_[0]->ProcessAnimationKey(0, glm::mat4(1.0f), cur_pose_,
                                           global_inverse_transform_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, bone_num_ * sizeof(glm::mat4),
                  glm::value_ptr(cur_pose_[0]));
}

void SkinnedObject3DImpl::MakeDrawable(unsigned int local_id) {
  drawable_instances_.insert(
    std::move(undrawable_instances_.extract(local_id)));
}
void SkinnedObject3DImpl::MakeUndrawable(unsigned int local_id) {
  undrawable_instances_.insert(
    std::move(drawable_instances_.extract(local_id)));
}

glm::mat4* SkinnedObject3DImpl::get_transform(unsigned int local_id) const {
  auto found_transform = drawable_instances_.find(local_id);
  if (found_transform != drawable_instances_.end())
    return found_transform->second.transform;

  found_transform = undrawable_instances_.find(local_id);
  if (found_transform != undrawable_instances_.end())
    return found_transform->second.transform;

  return nullptr;
}

void SkinnedObject3DImpl::RunAnimation(unsigned int local_id,
                                       int anim_id, bool repeat) {
  auto found_instance = drawable_instances_.find(local_id);
  if (found_instance == drawable_instances_.end()) {
    found_instance = undrawable_instances_.find(local_id);
    if (found_instance == drawable_instances_.end())
      return; // TODO: in general it's not possible because it manages by Me (dev)
  }
  animated_objects_.emplace_front(this, anim_id,
                                  found_instance->second.ubo, repeat);
}

void SkinnedObject3DImpl::Update(double framerate) {
  animated_objects_.remove_if(
    [framerate] (RunningAnimation &obj) {
    if (obj.Done())
      return true;
    obj.UpdateFrame(framerate);
    return false;
  });
}


unsigned int ObjectImplBase::last_global_id_ = 0;

/// preserve 16 binding points for further usage
///   (not 15 but 14 is because we use pre-increment)
unsigned int SkinnedObject3DImpl::last_ubo_bind_point_ = 14;

} // namespace faithful
