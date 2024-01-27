#ifndef FAITHFUL_SHADERPROGRAM_H
#define FAITHFUL_SHADERPROGRAM_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <iostream>
#include <memory>

#include "IAsset.h"

#include "ShaderObject.h"


/** How to use Shader:
   *
   * enum class ShaderMode {
   * VertFrag,
   * Computation,
   * VertGeomFrag,
   * TesselationGeom,
   * Tesselation,
   * etc...
   * }
   *
   * Shader sun_shader(ShaderMode::VertFrag,
   *                   "../assets/shaders/basic_shader.vert",
   *                   "../assets/shaders/basic_shader.frag");
   *
   * // and then just use:
   *
   * glUseProgram(sun_shader.GetShaderId());
   *
   *
   * TODO: reusing of glShaderObject (maybe ctor internally will check paths
   *    and in case of already processed just return ref/ptr to it, then
   *    after last shader processing programmer/user should call ClearCache())
 */


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
    // TODO:
    //  1) call glCreateProgram for __num__ shader programs
    //  2) load default shader program
    //  3) init free_instances_ with indices: 1,2,3,....,max_active_shader_program_num

    // if (!program_) return;
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
    baked_ = false;
    glAttachShader(opengl_id_, shader_obj.OpenglId());
  }
  void DetachShader(GLenum shader_type, const ShaderObject& shader_obj) {
    baked_ = false;
    glDetachShader(opengl_id_, 0); // TODO: second arg (do we need to store idx:type?)
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
  bool IsValidProgram() noexcept {
    if (!opengl_id_) {
      return false;
    }

    // TODO: am i sure for text below?

    int success;

    /// we're using mimalloc, so such allocation won't harm too much
    int buffer_size = 512;
    std::string buffer;
    buffer.reserve(buffer_size);

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
  /// if baked we can't attach shader objects
  bool baked_ = false;
};

}  // namespace faithful

#include "ShaderProgram-inl.h"

#endif  // FAITHFUL_SHADERPROGRAM_H
