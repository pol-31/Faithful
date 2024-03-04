#include "ShaderObjectPool.h"

#include <iostream>
#include <fstream>

#include "../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {
namespace assets {

ShaderObjectPool::DataType ShaderObjectPool::LoadImpl(
    TrackedDataType& instance_info) {
  int buffer_size = faithful::config::kShaderStartingBufferSize;
  std::string buffer;
  buffer.reserve(buffer_size);

  ReadToBuffer(instance_info.path, buffer);
  instance_info.data->type = DeduceShaderType(instance_info.path);
  instance_info.data->opengl_context = opengl_context_;

  opengl_context_->Put([&, buf = std::move(buffer), data = instance_info.data]{
    data->id = glCreateShader(data->type);
    glShaderSource(data->id, 1,
                   reinterpret_cast<const GLchar* const*>(buffer.c_str()),
                   nullptr);
    glCompileShader(data->id);

    CheckValidity(data->id, data->type, std::move(buffer));
    data->ready = true;
  });

  return instance_info.data;
}

GLenum ShaderObjectPool::DeduceShaderType(const std::string& shader_path) {
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
    return GL_VERTEX_SHADER;
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
      return GL_VERTEX_SHADER;
  }
}

void ShaderObjectPool::CheckValidity(GLuint shader, GLenum shader_type,
                                     std::string buffer) {
  if (!shader) {
    return;
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
  }
}


void ShaderObjectPool::ReadToBuffer(
    const std::string& path, std::string& buffer) {
  if (path.empty()) {
    return;
  }
  std::ifstream shader_file(path, std::ios::binary);

  if (!shader_file.is_open()) {
    std::cout << "Shader at " << path << " is invalid" << std::endl;
    return;
  }

  shader_file.seekg(0, std::ios::end);
  std::fpos shader_file_size = shader_file.tellg();

  shader_file.seekg(0, std::ios::beg);

  int needed_buffer_size = static_cast<int>(shader_file_size);

  // TODO: (class Buffer)
  if (buffer.size() < static_cast<std::basic_string<char>::size_type>(
                          needed_buffer_size)) {
    buffer.reserve(needed_buffer_size);
  }
  shader_file.read(buffer.data(), shader_file_size);
}

} // namespace assets
} // namespace details
} // namespace faithful

