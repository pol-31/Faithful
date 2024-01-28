#ifndef FAITHFUL_SHADERPROGRAM_H
#define FAITHFUL_SHADERPROGRAM_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <iostream>
#include <memory>

#include "IAsset.h"

#include "ShaderObject.h"


/* TODO: handle case:
 * typical scope failure scenario:
   * { // scope # 1
   *  ShaderProgram shp;
   *
   * { // scope # 2
   *   ShaderObject sho(vertex_type, "shader path")
   *   shp.AttachShader(sho);
   * } // -- ~sho();
   *  // --- there happened a lot of things, including Shader object Clearing
   *  shp.Bake(); < -- there shader object already invalid
   * }
   * */

namespace faithful {

namespace details {
namespace shader {

/// should be only 1 instance for the entire program
template <int max_active_shader_programs>
class ShaderProgramManager : public faithful::details::IAssetManager<max_active_shader_programs> {
 public:
  using Base = faithful::details::IAssetManager<max_active_shader_programs>;
  using InstanceInfo = typename Base::InstanceInfo;

  ShaderProgramManager() {
    for (auto& program : active_instances_) {
      program = {"", new details::AssetManagerRefCounter, glCreateProgram()};
      if (program.opengl_id == 0) {
        std::cerr << "shaderProgramManager() error in glCreateProgram()"
                  << std::endl;
        return;
      }
    }
    // TODO: load default shader program
    for (int i = 0; i < free_instances_.Size(); ++i) {
      free_instances_[i] = i;
    }
  }
  ~ShaderProgramManager() {
    for (auto& i : active_instances_) {
      glDeleteProgram(i.opengl_id_);
    }
  }

  /// not copyable
  ShaderProgramManager(const ShaderProgramManager&) = delete;
  ShaderProgramManager& operator=(const ShaderProgramManager&) = delete;

  /// movable
  ShaderProgramManager(ShaderProgramManager&&) = default;
  ShaderProgramManager& operator=(ShaderProgramManager&&) = default;

  InstanceInfo Load(std::string&&) {}

 private:
  using Base::active_instances_;
  using Base::free_instances_;

  int default_shader_program_id_ = 0; // adjust
};

} // namespace shader
} // namespace details


class ShaderProgram : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

  /// Base::Bind(GLenum) intentionally hided
  void Bind(GLenum target) {
    if (!baked_) {
      glLinkProgram(opengl_id_);
      // TODO: <---------------------
//      if (!IsValidProgram()) {
//        glDeleteProgram(program_);
//        program_ = 0;
//      }
      baked_ = true;
    }
    glUseProgram(opengl_id_);
  }

  void AttachShader(GLenum shader_type, const ShaderObject& shader_obj) {
    int shader_object_info_id = ShaderIdByShaderType(shader_type);
    if (shader_object_info_id == -1) return;
    // TODO: should delete previous data <----------------------------------------
    ShaderObjectInfo& instance = shader_object_info_[shader_object_info_id];
    instance.ref_counter = shader_obj.GetRefCounter();
    instance.opengl_id = shader_obj.OpenglId();
    baked_ = false;
    glAttachShader(opengl_id_, shader_obj.OpenglId());
  }
  void DetachShader(GLenum shader_type, const ShaderObject& shader_obj) {
    int shader_object_info_id = ShaderIdByShaderType(shader_type);
    if (shader_object_info_id == -1) return;
    baked_ = false;
    glDetachShader(opengl_id_,
                   shader_object_info_[shader_object_info_id].opengl_id);
  }

  inline void SetUniform(const GLchar* name, GLboolean v0);

  inline void SetUniform(const GLchar* name, GLfloat v0);

  inline void SetUniform(const GLchar* name, GLfloat v0, GLfloat v1);

  inline void SetUniform(const GLchar* name, GLfloat v0,
                         GLfloat v1, GLfloat v2);

  inline void SetUniform(const GLchar* name, GLfloat v0,
                         GLfloat v1, GLfloat v2, GLfloat v3);

  inline void SetUniform(const GLchar* name, GLint v0);

  inline void SetUniform(const GLchar* name, GLint v0, GLint v1);

  inline void SetUniform(const GLchar* name, GLint v0, GLint v1, GLint v2);

  inline void SetUniform(const GLchar* name, GLint v0, GLint v1,
                         GLint v2, GLint v3);

  inline void SetUniform(const GLchar* name, GLuint v0);

  inline void SetUniform(const GLchar* name, GLuint v0, GLuint v1);

  inline void SetUniform(const GLchar* name, GLuint v0, GLuint v1, GLuint v2);

  inline void SetUniform(const GLchar* name, GLuint v0, GLuint v1,
                         GLuint v2, GLuint v3);

  inline void SetUniformVec1(const GLchar* name, GLsizei count,
                             const GLfloat* value);

  inline void SetUniformVec2(const GLchar* name, GLsizei count,
                             const GLfloat* value);

  inline void SetUniformVec3(const GLchar* name, GLsizei count,
                             const GLfloat* value);

  inline void SetUniformVec4(const GLchar* name, GLsizei count,
                             const GLfloat* value);

  inline void SetUniformVec1(const GLchar* name, GLsizei count,
                             const GLint* value);

  inline void SetUniformVec2(const GLchar* name, GLsizei count,
                             const GLint* value);

  inline void SetUniformVec3(const GLchar* name, GLsizei count,
                             const GLint* value);

  inline void SetUniformVec4(const GLchar* name, GLsizei count,
                             const GLint* value);

  inline void SetUniformVec1(const GLchar* name, GLsizei count,
                             const GLuint* value);

  inline void SetUniformVec2(const GLchar* name, GLsizei count,
                             const GLuint* value);

  inline void SetUniformVec3(const GLchar* name, GLsizei count,
                             const GLuint* value);

  inline void SetUniformVec4(const GLchar* name, GLsizei count,
                             const GLuint* value);

  inline void SetUniformMat2v(const GLchar* name, GLsizei count,
                              GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat3v(const GLchar* name, GLsizei count,
                              GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat4v(const GLchar* name, GLsizei count,
                              GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat2x3v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat3x2v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat2x4v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat4x2v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat3x4v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value);

  inline void SetUniformMat4x3v(const GLchar* name, GLsizei count,
                                GLboolean transpose, const GLfloat* value);


 private:
  int ShaderIdByShaderType(GLenum shader_type) {
    switch (shader_type) {
      case GL_VERTEX_SHADER:
        return 0;
      case GL_FRAGMENT_SHADER:
        return 1;
      case GL_GEOMETRY_SHADER:
        return 2;
      case GL_TESS_CONTROL_SHADER:
        return 3;
      case GL_TESS_EVALUATION_SHADER:
        return 4;
      case GL_COMPUTE_SHADER:
        return 5;
      default:
        std::cerr << "ShaderProgram::ShaderIdByShaderType error in shader_type"
                  << std::endl;
        return -1;
    }
  }

  bool IsValidProgram() noexcept {
    if (!opengl_id_) {
      return false;
    }

    //   TODO: am i sure for text below?
    /// we're using mimalloc, so such allocation won't harm too much
    int buffer_size = 512;
    std::string buffer;
    buffer.reserve(buffer_size);
    
    int success;

    glGetProgramiv(opengl_id_, GL_LINK_STATUS, &success);

    // TODO:  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (!success) {
      glGetProgramInfoLog(global_id_, buffer_size, nullptr, buffer.data());
      std::cout << "Program linking error: " << buffer << "\n";
      return false;
    }
    return true;
  }

  using Base::opengl_id_;

  struct ShaderObjectInfo {
    GLuint opengl_id;
    details::AssetManagerRefCounter* ref_counter;
  };

  /// there is possible only 6 types of shader by now
  /// they comes in such order: vert, frag, geom, tess_control, tess_eval, comp
  std::array<ShaderObjectInfo, 6> shader_object_info_;

  /// if baked we can't attach shader objects
  bool baked_ = false;
};

}  // namespace faithful

#include "ShaderProgram-inl.h"

#endif  // FAITHFUL_SHADERPROGRAM_H
