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
    static_assert(max_active_shader_objects < 6,
                  "max_active_shader_objects should be at least 6");

    // TODO: very bad, need to adjust
    active_instances_[0] = {"",
                            new details::AssetManagerRefCounter,
                            glCreateShader(GL_VERTEX_SHADER)};
    active_instances_[1] = {"",
                            new details::AssetManagerRefCounter,
                            glCreateShader(GL_FRAGMENT_SHADER)};
    active_instances_[2] = {"",
                            new details::AssetManagerRefCounter,
                            glCreateShader(GL_GEOMETRY_SHADER)};
    active_instances_[3] = {"",
                            new details::AssetManagerRefCounter,
                            glCreateShader(GL_TESS_CONTROL_SHADER)};
    active_instances_[4] = {"",
                            new details::AssetManagerRefCounter,
                            glCreateShader(GL_TESS_EVALUATION_SHADER)};
    active_instances_[5] = {"",
                            new details::AssetManagerRefCounter,
                            glCreateShader(GL_COMPUTE_SHADER)};
    // TODO:
    //  (-) call glGenShader for __num__ shader objects
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
    /// we're using mimalloc, so such allocation won't harm too much
    int buffer_size = 512;
    std::string buffer;
    buffer.reserve(buffer_size);

    if (!ReadToBuffer(shader_path)) {
      return {"", nullptr, default_shader_object_id_};
    }
    auto shader_type = DeduceShaderType(shader_path);

    auto [shader_id, active_instances_id] = Base::AcquireId(); // structure binding
    if (shader_id == 0) {
      return {"", nullptr, default_shader_object_id_};
    }

    glShaderSource(shader_id, 1,
                   reinterpret_cast<const GLchar* const*>(buffer.c_str()),
                   nullptr);
    glCompileShader(shader_id);

    if (!IsValidShader(shader_id, shader_type,
                       buffer_size, std::move(buffer))) {
      return {"", nullptr, default_shader_object_id_};
    }
    return {shader_path, active_instances_[active_instances_id], shader_path};
  }

 private:
  GLenum DeduceShaderType(std::string&& shader_path) {
    /**
     * Vertex Shader: .vert
     * Fragment Shader: .frag
     * Geometry Shader: .geom
     * Tessellation Control Shader (TCS): .tesc
     * Tessellation Evaluation Shader (TES): .tese
     * Compute Shader: .comp
     */
    if (shader_path.length() <= 5) {
      // TODO: handle error there; return
      return 0;
    }
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
        // TODO: handle error there; return
        return 0;
    }
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
      glGetShaderInfoLog(shader, buffer_size, nullptr, buffer.data());
      std::cout << shader_name << " compiling error: " << buffer << "\n";
      return false;
    }
    return true;
  }


  bool ReadToBuffer(const std::string& path,
                    int buffer_size, std::string& buffer) noexcept {
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
    if (sizeof(buffer_size) < needed_buffer_size) {
      buffer.reserve(needed_buffer_size);
    }
    shader_file.read(buffer.data(), shader_file_size);
    return true;
  }

  using Base::active_instances_;
  using Base::free_instances_;

  int default_shader_object_id_ = 0; // adjust
};

} // namespace shader
} // namespace details


class ShaderObject : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

  // TODO: make accessible only to details::shader::ShaderObjectManager
  void SetShaderType(GLenum shader_type) {
    shader_type_ = shader_type;
  }

 protected:
  friend class ShaderProgram;
  details::AssetManagerRefCounter* GetRefCounter() const {
    return ref_counter_;
  }

 private:
  GLenum shader_type_;
  using Base::opengl_id_;
  using Base::ref_counter_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
