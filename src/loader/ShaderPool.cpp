#include "ShaderPool.h"

#include <iostream>
#include <fstream>

#include "../common/RefCounter.h"

namespace faithful {
namespace details {
namespace assets {

ShaderPool::ShaderPool() {
  for (auto& program : active_instances_) {
    auto program_id = static_cast<int>(glCreateProgram());
    program = {"", new details::RefCounter, program_id, program_id};
    if (program.internal_id == 0) {
      std::cerr << "shaderProgramManager() error in glCreateProgram()"
                << std::endl;
      return;
    }
  }
  active_shader_objects_.reserve(20); // not crucial, will be adjusted later
  // TODO: load 1-2 default shader objects
  // TODO: load default shader program
}

ShaderPool::~ShaderPool() {
  /// shader programs cleanup
  for (auto& i : active_instances_) {
    if (i.ref_counter->Active()) {
      std::cerr
          << "ShaderManager has been deleted but some ref_counters"
          << " for ShaderProgram still were active" << std::endl;
    }
    delete i.ref_counter;
    glDeleteProgram(i.internal_id);
  }

  /// shader objects cleanup (ref counters deleted in IAssetManager - Base class)
  for (auto& i : active_shader_objects_) {
    glDeleteShader(i.opengl_id);
  }
}

std::pair<AssetInstanceInfo, ShaderPool*> ShaderPool::CreateProgram() {
  auto [program_id, ref_counter, is_new_id] = Base::AcquireId("");
  if (program_id == -1) {
    return {{"", nullptr, default_id_, -1}, this};
  } else if (!is_new_id) {
    return {{"", nullptr,
             active_instances_[program_id].internal_id, program_id}, this};
  }
  return {{"", nullptr,
           active_instances_[program_id].internal_id, program_id}, this};
}

std::pair<AssetInstanceInfo, GLenum> ShaderPool::LoadShader(
    std::string&& shader_path) {
  // casting uint->int shouldn't cause any issues because
  // we don't use such numbers
  auto shader_id = static_cast<int>(glCreateShader(GL_VERTEX_SHADER));

  /// we're using mimalloc, so such allocation won't harm too much
  int buffer_size = faithful::config::shader_starting_buffer_size;
  std::string buffer;
  buffer.reserve(buffer_size);
  if (!ReadToBuffer(shader_path, buffer)) {
    return {{"", nullptr, default_id_, -1}, GL_VERTEX_SHADER};
  }
  auto shader_type = DeduceShaderType(shader_path);
  if (shader_type == static_cast<GLenum>(-1)) {
    return {{"", nullptr, default_id_, -1}, GL_VERTEX_SHADER};
  }

  glShaderSource(shader_id, 1,
                 reinterpret_cast<const GLchar* const*>(buffer.c_str()),
                 nullptr);
  glCompileShader(shader_id);

  // we move shader_path there, cause we don't need it anymore
  if (!IsValidShader(shader_id, shader_type, std::move(buffer))) {
    return {{"", nullptr, shader_id, shader_id}, shader_type};
  }
  auto ref_counter = new RefCounter;
  active_shader_objects_.push_back({ref_counter, shader_id});
  return {{"", nullptr, shader_id, shader_id}, shader_type};
}

void ShaderPool::ScanRefCounters() {
  for (auto& i : active_shader_objects_) {
    if (!i.ref_counter->Active()) {
      delete i.ref_counter;
      glDeleteShader(i.opengl_id);
    }
  }
}

GLenum ShaderPool::DeduceShaderType(const std::string& shader_path) {
  /** Shader naming (TODO: move to docs)
     * Vertex Shader: .vert
     * Fragment Shader: .frag
     * Geometry Shader: .geom
     * Tessellation Control Shader (TCS): .tesc
     * Tessellation Evaluation Shader (TES): .tese
     * Compute Shader: .comp
   */
  if (shader_path.length() <= 5) {
    std::cerr
        << "ShaderObjectManager::DeduceShaderType shader_path error 1"
        << std::endl;
    // TODO: log
    return static_cast<GLenum >(-1);
  }
  /// look at last letter of shader extension
  switch (shader_path.back()) {
    case 't':
      return GL_VERTEX_SHADER;
    case 'g':
      return GL_FRAGMENT_SHADER;
    case 'm':
      return GL_GEOMETRY_SHADER;
    case 'c':
      return GL_TESS_CONTROL_SHADER;
    case 'e':
      return GL_TESS_EVALUATION_SHADER;
    case 'p':
      return GL_COMPUTE_SHADER;
    default:
      std::cerr
          << "ShaderObjectManager::DeduceShaderType shader_path error 2"
          << std::endl;
      return static_cast<GLenum >(-1);
  }
}

bool ShaderPool::IsValidShader(
    GLuint shader, GLenum shader_type, std::string&& buffer) noexcept {
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
    case GL_TESS_CONTROL_SHADER:
      shader_name = "Tessellation control shader";
      break;
    case GL_TESS_EVALUATION_SHADER:
      shader_name = "Tesselation evaluation shader";
      break;
    case GL_COMPUTE_SHADER:
      shader_name = "Compute shader";
      break;
    default:
      shader_name = "Shader";
  }

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

  if (!success) {
    glGetShaderInfoLog(shader, buffer.size(), nullptr, buffer.data());
    std::cout << shader_name << " compiling error: " << buffer << "\n";
    return false;
  }
  return true;
}


bool ShaderPool::ReadToBuffer(
    const std::string& path, std::string& buffer) noexcept {
  if (path.empty()) {
    return false;
  }
  std::ifstream shader_file(path, std::ios::binary);

  if (!shader_file.is_open()) {
    [[unlikely]] std::cout << "Shader at " << path << " is invalid"
                           << std::endl;
    return false;
  }

  shader_file.seekg(0, std::ios::end);
  std::fpos shader_file_size = shader_file.tellg();

  shader_file.seekg(0, std::ios::beg);

  int needed_buffer_size = static_cast<int>(shader_file_size);

  // TODO: (class Buffer)
  if (sizeof(buffer.size()) < needed_buffer_size) {
    buffer.reserve(needed_buffer_size);
  }
  shader_file.read(buffer.data(), shader_file_size);
  return true;
}

} // namespace assets
} // namespace details
} // namespace faithful

