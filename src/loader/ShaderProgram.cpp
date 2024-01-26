#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <new>

namespace faithful {

namespace details {
namespace shader {

ShaderProgramManager::ShaderProgramManager() {
}

ShaderProgramManager::ShaderProgramManager(const char* vertex_shader_path,
                             const char* fragment_shader_path,
                             const char* geometry_shader_path) noexcept {
  buffer_ = new (std::nothrow) char[256];
  if (!buffer_) {
    return;
  }
  buffer_size_ = 256;

  program_ = glCreateProgram();
  if (!program_) {
    return;
  }

  AttachShader(vertex_shader_path, GL_VERTEX_SHADER);
  AttachShader(fragment_shader_path, GL_FRAGMENT_SHADER);
  AttachShader(geometry_shader_path, GL_GEOMETRY_SHADER);

  glLinkProgram(program_);
  if (!IsValidProgram()) {
    glDeleteProgram(program_);
    program_ = 0;
  }
}

ShaderProgramManager::~ShaderProgramManager() {
  glDeleteProgram(program_);
  delete[] buffer_;
}

bool ShaderProgramManager::IsValidShader(GLuint shader, GLenum shader_type) noexcept {
  if (!shader) {
    return false;
  }

  std::string_view shader_name;

  switch (shader_type) {
    case GL_VERTEX_SHADER:
      shader_name = "Vertex shader";
      break;
    case GL_FRAGMENT_SHADER:
      shader_name = "Fragment shader";
      break;
    case GL_GEOMETRY_SHADER:
      shader_name = "Geometry shader";
      break;
    default:
      shader_name = "Shader";
  }

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success_);

  // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

  if (!success_) {
    glGetShaderInfoLog(shader, buffer_size_, nullptr, buffer_);
    std::cout << shader_name << " compiling error: " << buffer_ << "\n";
    return false;
  }
  return true;
}

bool ShaderProgramManager::IsValidProgram() noexcept {
  if (!program_) {
    return false;
  }

  glGetProgramiv(program_, GL_LINK_STATUS, &success_);

  // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (!success_) {
    glGetProgramInfoLog(program_, buffer_size_, nullptr, buffer_);
    std::cout << "Program linking error: " << buffer_ << "\n";
    return false;
  }
  return true;
}

void ShaderProgramManager::AttachShader(const char* path,
                                 GLenum shader_type) noexcept {
  if (!ReadToBuffer(path)) {
    return;
  }

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &buffer_, nullptr);
  glCompileShader(shader);

  if (IsValidShader(shader, shader_type)) {
    glAttachShader(program_, shader);
  }
  glDeleteShader(shader);
}

bool ShaderProgramManager::ReadToBuffer(const char* path) noexcept {
  if (!path) {
    [[unlikely]] return false;
  }
  std::ifstream shader_file(path, std::ios::binary);

  if (!shader_file.is_open()) {
    [[unlikely]] std::cout << "Shader at " << path << " is invalid"
                           << std::endl;
    return false;
  }

  // getting file size
  shader_file.seekg(0, std::ios::end);
  std::fpos shader_file_size = shader_file.tellg();

  shader_file.seekg(0, std::ios::beg);

  // TODO: (class Buffer)
  if (sizeof(buffer_) < (static_cast<int>(shader_file_size) + 1)) {
    delete[] buffer_;
    buffer_ = new (std::nothrow) char[static_cast<int>(shader_file_size) + 1];
    if (!buffer_) {
      return false;
    }
    buffer_size_ = static_cast<int>(shader_file_size) + 1;
  }

  shader_file.read(buffer_, shader_file_size);
  buffer_[shader_file_size] = '\0';

  return true;
}

/*glUniformBlockBinding(object3d_->Id(),
                        glGetUniformBlockIndex(object3d_->Id(), "Matrices"), 0);

  glGenBuffers(1, &ubo_matrices_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices_, 0,
                  2 * sizeof(glm::mat4));

glm::mat4 projection =
    glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_);
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                glm::value_ptr(projection));
glBindBuffer(GL_UNIFORM_BUFFER, 0);
*/


void ShaderProgramManager::ReuseShaderProgram(int opengl_id) {
  // similar to TextureManager
}
void ShaderProgramManager::Restore(int opengl_id) {
  // similar to TextureManager
}


} // namespace shader
} // namespace details



void ShaderProgram::Bake() {

  // compile, check for errors
}

void ShaderProgram::AttachShader(GLenum shader_type,
                                 const ShaderObject& shader_obj) {
  glAttachShader(id_, shader_obj.Id());
  manager_->IsValidShader(id_, shader_type);
  // attach, check for erorrs
}

void ShaderProgram::Use() {
  if (id_) {
    glUseProgram(id_);
  }
}

}  // namespace faithful
