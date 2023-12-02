#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <new>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace faithful {


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/*
namespace utility {



ShaderProgram::ShaderProgram() {}

ShaderProgram::ShaderProgram(
  const char* vertex_shader_path,
  const char* fragment_shader_path,
  const char* geometry_shader_path) noexcept {

  buffer_ = new(std::nothrow) char[256];
  if (!buffer_) return;
  buffer_size_ = 256;

  program_ = glCreateProgram();
  if (!program_)
    return;

  AttachShader(vertex_shader_path, GL_VERTEX_SHADER);
  AttachShader(fragment_shader_path, GL_FRAGMENT_SHADER);
  AttachShader(geometry_shader_path, GL_GEOMETRY_SHADER);

  glLinkProgram(program_);
  if (!IsValidProgram()) {
    glDeleteProgram(program_);
    program_ = 0;
  }
}

ShaderProgram& ShaderProgram::operator= (ShaderProgram&& sp) noexcept {
  if (program_ == sp.get_program()) {
    // TODO: (class Logger)
    [[unlikely]]
    return *this;
  }
  program_ = sp.get_program();
  buffer_ = sp.get_buffer();
  sp.set_buffer(nullptr);
  return *this;
}

ShaderProgram::ShaderProgram(ShaderProgram&& sp) noexcept : program_(sp.get_program()) {
buffer_ = sp.get_buffer();
sp.set_buffer(nullptr);
}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(program_);
  delete[] buffer_;
}

void ShaderProgram::set_buffer(char* buffer) noexcept {
  delete[] buffer_;
  buffer_ = buffer;
}

bool ShaderProgram::IsValidShader(GLuint shader, GLenum shader_type) noexcept {
  if (!shader) return false;

  std::string_view shader_name;

  switch(shader_type) {
    case GL_VERTEX_SHADER:
      shader_name = "Vertex shader";
      break;
    case GL_FRAGMENT_SHADER:
      shader_name =  "Fragment shader";
      break;
    case GL_GEOMETRY_SHADER:
      shader_name = "Geometry shader";
      break;
    default:
      shader_name = "Shader";
  }

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success_);
  if (!success_) {
    glGetShaderInfoLog(shader, buffer_size_, nullptr, buffer_);
    std::cout << shader_name << " compiling error: " << buffer_ << "\n";
    return false;
  }
  return true;
}

bool ShaderProgram::IsValidProgram() noexcept {
  if (!program_) return false;

  glGetProgramiv(program_, GL_LINK_STATUS, &success_);
  if (!success_) {
    glGetProgramInfoLog(program_, buffer_size_, nullptr, buffer_);
    std::cout << "Program linking error: " << buffer_ << "\n";
    return false;
  }
  return true;
}

void ShaderProgram::AttachShader(const char* path, GLenum shader_type) noexcept {
  if (!ReadToBuffer(path)) { return; }

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &buffer_, nullptr);
  glCompileShader(shader);

  if (IsValidShader(shader, shader_type)) {
    glAttachShader(program_, shader);
  }
  glDeleteShader(shader);
}

bool ShaderProgram::ReadToBuffer(const char* path) noexcept {
  if (!path) {
    [[unlikely]]
    return false;
  }
  std::ifstream shader_file(path, std::ios::binary);

  if (!shader_file.is_open()) {
    [[unlikely]]
      std::cout << "Shader at " << path << " is invalid" << std::endl;
    return false;
  }

  // getting file size
  shader_file.seekg(0, std::ios::end);
  std::fpos shader_file_size = shader_file.tellg();

  shader_file.seekg(0, std::ios::beg);

  // TODO: (class Buffer)
  if (sizeof(buffer_) < (static_cast<int>(shader_file_size) + 1)) {
    delete[] buffer_;
    buffer_ = new(std::nothrow) char[static_cast<int>(shader_file_size) + 1];
    if (!buffer_) return false;
    buffer_size_ = static_cast<int>(shader_file_size) + 1;
  }

  shader_file.read(buffer_, shader_file_size);
  buffer_[shader_file_size] = '\0';

  return true;
}

void DefaultShaderProgram::Init() {
  if (initialized_) return;

  buffer_ = new(std::nothrow) char[256];
  if (!buffer_) return;
  buffer_size_ = 256;

  object3d_ = new ShaderProgram;
  program_ = glCreateProgram();
  AttachShader(object_vertex_shader_path_, GL_VERTEX_SHADER);
  AttachShader(object_fragment_shader_path_, GL_FRAGMENT_SHADER);
  glLinkProgram(program_);
  object3d_->set_program(program_);


  cubemap_ = new ShaderProgram;
  program_ = glCreateProgram();
  AttachShader(cubemap_vertex_shader_path_, GL_VERTEX_SHADER);
  AttachShader(cubemap_fragment_shader_path_, GL_FRAGMENT_SHADER);
  glLinkProgram(program_);
  cubemap_->set_program(program_);


  object2d_ = new ShaderProgram;
  program_ = glCreateProgram();
  AttachShader(hud_vertex_shader_path_, GL_VERTEX_SHADER);
  AttachShader(hud_fragment_shader_path_, GL_FRAGMENT_SHADER);
  glLinkProgram(program_);
  object2d_->set_program(program_);

  hud_stencil_shader_program_ = new ShaderProgram;
  program_ = glCreateProgram();
  AttachShader(hud_stencil_vertex_shader_path_, GL_VERTEX_SHADER);
  AttachShader(hud_stencil_fragment_shader_path_, GL_FRAGMENT_SHADER);
  glLinkProgram(program_);
  hud_stencil_shader_program_->set_program(program_);

  picking_object_shader_program_ = new ShaderProgram;
  program_ = glCreateProgram();
  AttachShader(picking_object_vertex_shader_path_, GL_VERTEX_SHADER);
  AttachShader(picking_object_fragment_shader_path_, GL_FRAGMENT_SHADER);
  glLinkProgram(program_);
  picking_object_shader_program_->set_program(program_);

  object3d_skinned_ = new ShaderProgram;
  program_ = glCreateProgram();
  AttachShader(bone_3d_vertex_shader_path_, GL_VERTEX_SHADER);
  AttachShader(bone_3d_fragment_shader_path_, GL_FRAGMENT_SHADER);
  glLinkProgram(program_);
  object3d_skinned_->set_program(program_);

  ReadToBuffer(bone_3d_vertex_shader_path_);
  bone_vectex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(bone_vectex_shader_, 1, &buffer_, nullptr);
  glCompileShader(bone_vectex_shader_);
  ReadToBuffer(bone_3d_fragment_shader_path_);
  bone_fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(bone_fragment_shader_, 1, &buffer_, nullptr);
  glCompileShader(bone_fragment_shader_);

  ReadToBuffer(bone_3d_vertex_shader_path_);
  bone_light_vectex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(bone_light_vectex_shader_, 1, &buffer_, nullptr);
  glCompileShader(bone_light_vectex_shader_);
  ReadToBuffer(bone_3d_fragment_shader_path_);
  bone_light_fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(bone_light_fragment_shader_, 1, &buffer_, nullptr);
  glCompileShader(bone_light_fragment_shader_);

  ReadToBuffer(bone_3d_vertex_shader_path_);
  bone_light_parallax_vectex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(bone_light_parallax_vectex_shader_, 1, &buffer_, nullptr);
  glCompileShader(bone_light_parallax_vectex_shader_);
  ReadToBuffer(bone_3d_fragment_shader_path_);
  bone_light_parallax_fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(bone_light_parallax_fragment_shader_, 1, &buffer_, nullptr);
  glCompileShader(bone_light_parallax_fragment_shader_);

  //delete[] buffer_;
  //buffer_ = nullptr;
  //buffer_size_ = 0;

  glUniformBlockBinding(
    object3d_->get_program(),
    glGetUniformBlockIndex(object3d_->get_program(), "Matrices"),
    0);

  glUniformBlockBinding(
    cubemap_->get_program(),
    glGetUniformBlockIndex(cubemap_->get_program(), "Matrices"),
    0);

  glUniformBlockBinding(
    picking_object_shader_program_->get_program(),
    glGetUniformBlockIndex(picking_object_shader_program_->get_program(), "Matrices"),
    0);

  glUniformBlockBinding(
    object3d_skinned_->get_program(),
    glGetUniformBlockIndex(object3d_skinned_->get_program(), "Matrices"),
    0);

  object3d_light_ = object3d_;
  object3d_light_parallax_ = object3d_;

  glGenBuffers(1, &ubo_matrices_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices_, 0, 2 * sizeof(glm::mat4));

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  initialized_ = true;
}

bool DefaultShaderProgram::ReadToBuffer(const char* path) noexcept {
  if (!path) {
    [[unlikely]]
    return false;
  }
  std::ifstream shader_file(path, std::ios::binary);

  if (!shader_file.is_open()) {
    [[unlikely]]
      std::cout << "Shader at " << path << " is invalid" << std::endl;
    return false;
  }

  // getting file size
  shader_file.seekg(0, std::ios::end);
  std::fpos shader_file_size = shader_file.tellg();

  shader_file.seekg(0, std::ios::beg);

  // TODO: (class Buffer)
  if (sizeof(buffer_) < (static_cast<int>(shader_file_size) + 1)) {
    delete[] buffer_;
    buffer_ = new(std::nothrow) char[static_cast<int>(shader_file_size) + 1];
    if (!buffer_) return false;
    buffer_size_ = static_cast<int>(shader_file_size) + 1;
  }

  shader_file.read(buffer_, shader_file_size);
  buffer_[shader_file_size] = '\0';

  return true;
}

bool DefaultShaderProgram::IsValidProgram() noexcept {
  if (!program_) return false;

  glGetProgramiv(program_, GL_LINK_STATUS, &success_);
  if (!success_) {
    glGetProgramInfoLog(program_, buffer_size_, nullptr, buffer_);
    std::cout << "Program linking error: " << buffer_ << "\n";
    return false;
  }
  return true;
}


bool DefaultShaderProgram::IsValidShader(GLuint shader, GLenum shader_type) noexcept {
  if (!shader) return false;

  std::string_view shader_name;

  switch(shader_type) {
    case GL_VERTEX_SHADER:
      shader_name = "Vertex shader";
      break;
    case GL_FRAGMENT_SHADER:
      shader_name =  "Fragment shader";
      break;
    default:
      shader_name = "Shader";
  }

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success_);
  if (!success_) {
    glGetShaderInfoLog(shader, buffer_size_, nullptr, buffer_);
    std::cout << shader_name << " compiling error: " << buffer_ << "\n";
    return false;
  }
  return true;
}

void DefaultShaderProgram::AttachShader(const char* path, GLenum shader_type) noexcept {
  if (!ReadToBuffer(path)) { return; }

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &buffer_, nullptr);
  glCompileShader(shader);

  if (IsValidShader(shader, shader_type)) {
    glAttachShader(program_, shader);
  }
  glDeleteShader(shader);
}

ShaderProgram* DefaultShaderProgram::CreateBoneProgram() {
  auto new_program = new ShaderProgram;
  unsigned int new_program_id = 0;
  new_program_id = glCreateProgram();
  glAttachShader(new_program_id, bone_vectex_shader_);
  glAttachShader(new_program_id, bone_fragment_shader_);
  glLinkProgram(new_program_id);
  new_program->set_program(new_program_id);

  // TODO: is this correct
  glUniformBlockBinding(new_program_id,
                        glGetUniformBlockIndex(new_program_id, "Matrices"),
                        0);

  return new_program;
}

unsigned int DefaultShaderProgram::bone_vectex_shader_;
unsigned int DefaultShaderProgram::bone_light_vectex_shader_;
unsigned int DefaultShaderProgram::bone_light_parallax_vectex_shader_;

unsigned int DefaultShaderProgram::bone_fragment_shader_;
unsigned int DefaultShaderProgram::bone_light_fragment_shader_;
unsigned int DefaultShaderProgram::bone_light_parallax_fragment_shader_;



ShaderProgram* DefaultShaderProgram::object3d_ = nullptr;
ShaderProgram* DefaultShaderProgram::picking_object_shader_program_ = nullptr;
ShaderProgram* DefaultShaderProgram::cubemap_ = nullptr;
ShaderProgram* DefaultShaderProgram::object2d_ = nullptr;
ShaderProgram* DefaultShaderProgram::hud_stencil_shader_program_ = nullptr;
ShaderProgram* DefaultShaderProgram::object3d_skinned_ = nullptr;

ShaderProgram* DefaultShaderProgram::object3d_skinned_light_ = nullptr;
ShaderProgram* DefaultShaderProgram::object3d_skinned_light_parallax_ = nullptr;

ShaderProgram* DefaultShaderProgram::object3d_light_ = nullptr;
ShaderProgram* DefaultShaderProgram::object3d_light_parallax_ = nullptr;

bool DefaultShaderProgram::initialized_ = false;
// precaution: path relative to cmake-build-debug
char* DefaultShaderProgram::object_vertex_shader_path_ = "../assets/shaders/shader.vs";
char* DefaultShaderProgram::object_fragment_shader_path_ = "../assets/shaders/shader.fs";
char* DefaultShaderProgram::cubemap_vertex_shader_path_ = "../assets/shaders/cubemap.vs";
char* DefaultShaderProgram::cubemap_fragment_shader_path_ = "../assets/shaders/cubemap.fs";
char* DefaultShaderProgram::hud_vertex_shader_path_ = "../assets/shaders/shader_2d.vs";
char* DefaultShaderProgram::hud_fragment_shader_path_ = "../assets/shaders/shader_2d.fs";
char* DefaultShaderProgram::picking_object_vertex_shader_path_ = "../assets/shaders/shader.vs";
char* DefaultShaderProgram::picking_object_fragment_shader_path_ = "../assets/shaders/shader_picking.fs";
char* DefaultShaderProgram::hud_stencil_vertex_shader_path_ = "../assets/shaders/shader_2d.vs";
char* DefaultShaderProgram::hud_stencil_fragment_shader_path_ = "../assets/shaders/shader_2d_stencil.fs";
char* DefaultShaderProgram::bone_3d_vertex_shader_path_ = "../assets/shaders/shader_bone.vs";
char* DefaultShaderProgram::bone_3d_fragment_shader_path_ = "../assets/shaders/shader.fs";

char* DefaultShaderProgram::buffer_ = nullptr;
GLint DefaultShaderProgram::buffer_size_ = 0;
GLint DefaultShaderProgram::success_ = 0;
GLuint DefaultShaderProgram::program_ = 0;
GLuint DefaultShaderProgram::ubo_matrices_ = 0;


} // namespace utility*/
} // namespace faithful
