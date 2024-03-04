#include "DrawManager.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace faithful {
namespace details {

DrawManager::DrawManager(assets::ModelPool* model_manager)
    : model_manager_(model_manager) {}


void DrawManager::Init() {
  // TODO: add ALL shader_program, ubo creation to DrawManager::Init()

  unsigned int new_ubo;
  glGenBuffers(1, &new_ubo);
  auto new_shader_program = utility::DefaultShaderProgram::CreateBoneProgram();

  glBindBuffer(GL_UNIFORM_BUFFER, new_ubo);
  glBufferData(GL_UNIFORM_BUFFER, 200 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

  GLuint bindingPoint = ++last_ubo_bind_point_;
  glUniformBlockBinding(new_shader_program->get_program(),
                        glGetUniformBlockIndex(new_shader_program->get_program(), "BoneData"),
                        bindingPoint);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, new_ubo);
}

void DrawManager::Update() {
  // TODO: process frame + all from task_queue_
}



} // namespace details
} // namespace faithful
