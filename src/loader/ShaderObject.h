#ifndef FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
#define FAITHFUL_SRC_LOADER_SHADEROBJECT_H_

#include <fstream>
#include <string>

#include <glad/glad.h>

#include "IAsset.h"

namespace faithful {

class ShaderObject;

namespace details {
namespace shader {

template <int max_active_shader_objects>
class ShaderObjectManager : public faithful::details::IAssetManager<max_active_shader_objects> {
 public:
  using Base = faithful::details::IAssetManager<max_active_shader_objects>;
  using InstanceInfo = typename Base::InstanceInfo;

  ShaderObjectManager() {
    // TODO:
    //  1) call glGenShader for __num__ shader objects
    //  2) load 1-2 default shader objects
    //  3) init free_instances_ with indices: 1,2,3,....,max_active_shader_objects_num
    //  4) allocate buffer_; bool success_, int buffer_size_

    // if (!buffer_) return;
  }

  ~ShaderObjectManager() {
    // TODO: deallocate buffer (e.g. delete[] buffer_)
    for (auto& i : active_instances_) {
      glDeleteShader(i.opengl_id_);
    }
  }

  /// not copyable
  ShaderObjectManager(const ShaderObjectManager&) = delete;
  ShaderObjectManager& operator=(const ShaderObjectManager&) = delete;

  /// movable
  ShaderObjectManager(ShaderObjectManager&&) = default;
  ShaderObjectManager& operator=(ShaderObjectManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& shader_path) {

    int buffer_size = 512;
    std::string buffer;
    buffer.reserve(buffer_size);

    if (!ReadToBuffer(shader_path)) {
      return;
    }
    auto shader_type = DeduceShaderType(shader_path);
    GLuint shader = glCreateShader(shader_type);

    /// we're using mimalloc, so such allocation won't harm too much
    glShaderSource(shader, 1, buffer.data(), nullptr);
    glCompileShader(shader);

    InstanceInfo instance_info;

    if (!IsValidShader()) {
      instance_info.opengl_id = default_shader_object_id_;
      instance_info.ref_counter = nullptr;
      shader_path = nullptr;
    } else {
      // TODO: reuse ref_counter (see Texture.h)
      // TODO: reuse opengl_id (see Texture.h)
      instance_info.path = shader_path;
    }
  }

 private:
  GLenum DeduceShaderType(std::string&& shader_path) {
    // TODO: .vert, .frag, etc...
    std::cerr << "DeduceShaderType(std::string&&) not implemented"
              << std::endl;
  }

  bool IsValidShader(GLuint shader, GLenum shader_type,
                     int buffer_size, std::string&& buffer) noexcept {
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

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (!success) {
      glGetShaderInfoLog(shader, buffer_size, nullptr, buffer.data());
      std::cout << shader_name << " compiling error: " << buffer << "\n";
      return false;
    }
    return true;
  }


 // TODO: buffer usage totally incorrect <---------------------
  bool ReadToBuffer(const char* path,
                    int buffer_size, std::string& buffer) noexcept {
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
    if (sizeof(buffer) < (static_cast<int>(shader_file_size) + 1)) {
      delete[] buffer_;
      buffer_ = new (std::nothrow) char[static_cast<int>(shader_file_size) + 1];
      if (!buffer_) {
        return false;
      }
      buffer_size_ = static_cast<int>(shader_file_size) + 1;
    }

    shader_file.read(buffer.c_str(), shader_file_size);
    buffer[shader_file_size] = '\0';

    return true;
  }

  using Base::active_instances_;
  using Base::free_instances_;

  int default_shader_object_id_ = 0; // adjust
};

} // namespace shader
} // namespace details


// TODO: somehow integrate shader type (vertex, fragment, geometry, etc...)
class ShaderObject : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

 private:
  // TODO: type?
  using Base::opengl_id_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
