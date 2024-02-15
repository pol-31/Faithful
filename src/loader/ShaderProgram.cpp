#include "ShaderProgram.h"

#include <iostream>
#include <string>

#include "ShaderPool.h"
#include "../common/RefCounter.h"

namespace faithful {

ShaderProgram::ShaderProgram(
    details::RefCounter* ref_counter, GLuint id,
    details::assets::ShaderPool* shader_manager) {
  ref_counter_ = ref_counter;
  ref_counter_->Increment();
  internal_id_ = id;
  shader_manager_ = shader_manager;
}

void ShaderProgram::Bind() {
  if (!baked_) {
    glLinkProgram(internal_id_);
    shaders_.vertex = ShaderObject{};
    shaders_.fragment = ShaderObject{};
    shaders_.geometry = ShaderObject{};
    shaders_.tessellation_control = ShaderObject{};
    shaders_.tessellation_evaluation = ShaderObject{};
    shaders_.compute = ShaderObject{};
    shader_manager_->ScanRefCounters();
    if (!IsValidProgram()) {
      //      glDeleteProgram(opengl_id_); TODO: < -------- idk
      return;
    }
    baked_ = true;
  }
  glUseProgram(internal_id_);
}

void ShaderProgram::AttachShader(const ShaderObject& shader_obj) {
  GLenum shader_type = shader_obj.GetShaderType();
  switch (shader_type) {
    case GL_VERTEX_SHADER:
      shaders_.vertex = shader_obj;
      break;
    case GL_FRAGMENT_SHADER:
      shaders_.fragment = shader_obj;
      break;
    case GL_GEOMETRY_SHADER:
      shaders_.geometry = shader_obj;
      break;
    case GL_TESS_CONTROL_SHADER:
      shaders_.tessellation_control = shader_obj;
      break;
    case GL_TESS_EVALUATION_SHADER:
      shaders_.tessellation_evaluation = shader_obj;
      break;
    case GL_COMPUTE_SHADER:
      shaders_.compute = shader_obj;
      break;
    default:
      std::cerr << "ShaderProgram::AttachShader incorrect shader type"
                << std::endl;
      return;
  }
  shader_manager_->ScanRefCounters();
  baked_ = false;
  glAttachShader(internal_id_, shader_obj.GetInternalId());
}
void ShaderProgram::DetachShader(const ShaderObject& shader_obj) {
  baked_ = false;
  glDetachShader(internal_id_, shader_obj.GetInternalId());
  GLenum shader_type = shader_obj.GetShaderType();
  switch (shader_type) {
      /// assigning or ShaderObject{} safely reset ref_counter and release id
    case GL_VERTEX_SHADER:
      shaders_.vertex = ShaderObject{};
      break;
    case GL_FRAGMENT_SHADER:
      shaders_.fragment = ShaderObject{};
      break;
    case GL_GEOMETRY_SHADER:
      shaders_.geometry = ShaderObject{};
      break;
    case GL_TESS_CONTROL_SHADER:
      shaders_.tessellation_control = ShaderObject{};
      break;
    case GL_TESS_EVALUATION_SHADER:
      shaders_.tessellation_evaluation = ShaderObject{};
      break;
    case GL_COMPUTE_SHADER:
      shaders_.compute = ShaderObject{};
      break;
    default:
      // TODO: unreachable attribute
      std::cerr << "ShaderProgram::DetachShader incorrect shader type"
                << std::endl;
      return;
  }
  shader_manager_->ScanRefCounters();
}

bool ShaderProgram::IsValidProgram() noexcept {
  if (!internal_id_) {
    return false;
  }

  /// we're using mimalloc, so such allocation won't harm too much
  int buffer_size = 512;
  std::string buffer;
  buffer.reserve(buffer_size);

  int success;

  glGetProgramiv(internal_id_, GL_LINK_STATUS, &success);

  // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (!success) {
    glGetProgramInfoLog(external_id_, buffer_size, nullptr, buffer.data());
    std::cout << "Program linking error: " << buffer << "\n";
    return false;
  }
  return true;
}

} // namespace faithful
