#include "ShaderProgram.h"

#include "../executors/DisplayInteractionThreadPool.h"
#include "ShaderObjectPool.h"

#include <iostream>
#include <string>

namespace faithful {

void ShaderProgram::SetShaderObjectPool(
    details::assets::ShaderObjectPool* shader_object_pool) {
  shader_object_pool_ = shader_object_pool;
}

void ShaderProgram::Bind() {
  if (!data_->baked) {
    glLinkProgram(data_->id);
    data_->shaders.vertex = ShaderObject{};
    data_->shaders.fragment = ShaderObject{};
    data_->shaders.geometry = ShaderObject{};
    data_->shaders.tessellation_control = ShaderObject{};
    data_->shaders.tessellation_evaluation = ShaderObject{};
    data_->shaders.compute = ShaderObject{};
    shader_object_pool_->ClearInactive();
    CheckValidity(data_->id);
    data_->baked = true;
  }
  glUseProgram(data_->id);
}

void ShaderProgram::AttachShader(const ShaderObject& shader_obj) {
  GLenum shader_type = shader_obj.GetType();
  switch (shader_type) {
    case GL_VERTEX_SHADER:
      data_->shaders.vertex = shader_obj;
      break;
    case GL_FRAGMENT_SHADER:
      data_->shaders.fragment = shader_obj;
      break;
    case GL_GEOMETRY_SHADER:
      data_->shaders.geometry = shader_obj;
      break;
    case GL_TESS_CONTROL_SHADER:
      data_->shaders.tessellation_control = shader_obj;
      break;
    case GL_TESS_EVALUATION_SHADER:
      data_->shaders.tessellation_evaluation = shader_obj;
      break;
    case GL_COMPUTE_SHADER:
      data_->shaders.compute = shader_obj;
      break;
    default:
      std::cerr << "ShaderProgram::AttachShader incorrect shader type"
                << std::endl;
      return;
  }
  shader_object_pool_->ClearInactive();
  data_->baked = false;
  opengl_context_->Put([=] {
    glAttachShader(data_->id, shader_obj.GetId());
  });
}

void ShaderProgram::DetachShader(const ShaderObject& shader_obj) {
  data_->baked = false;
  GLenum shader_type = shader_obj.GetType();
  switch (shader_type) {
      /// assigning or ShaderObject{} safely reset ref_counter and release id
    case GL_VERTEX_SHADER:
      data_->shaders.vertex = ShaderObject{};
      break;
    case GL_FRAGMENT_SHADER:
      data_->shaders.fragment = ShaderObject{};
      break;
    case GL_GEOMETRY_SHADER:
      data_->shaders.geometry = ShaderObject{};
      break;
    case GL_TESS_CONTROL_SHADER:
      data_->shaders.tessellation_control = ShaderObject{};
      break;
    case GL_TESS_EVALUATION_SHADER:
      data_->shaders.tessellation_evaluation = ShaderObject{};
      break;
    case GL_COMPUTE_SHADER:
      data_->shaders.compute = ShaderObject{};
      break;
  }
  shader_object_pool_->ClearInactive();

  opengl_context_->Put([=] {
    glDetachShader(data_->id, shader_obj.GetId());
  });
}

void ShaderProgram::CheckValidity(GLuint id) {
  if (id == 0) {
    return;
  }
  int buffer_size = 512;
  std::string buffer;
  buffer.reserve(buffer_size);

  int success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);

  // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (!success) {
    glGetProgramInfoLog(id, buffer_size, nullptr, buffer.data());
    std::cout << "Program linking error: " << buffer << "\n";
  }
}

} // namespace faithful
