#ifndef FAITHFUL_SHADERPROGRAM_H
#define FAITHFUL_SHADERPROGRAM_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <iostream>

#include "ResourceManager.h"


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

namespace faithful {

class ShaderObject;
class ShaderProgram;

namespace details {
namespace shader {

/// should be only 1 instance for the entire program
template <int max_active_shader_programs>
class ShaderProgramManager : public faithful::details::IResourceManager<max_active_shader_programs> {
 public:
  using Base = faithful::details::IResourceManager<max_active_shader_programs>;
  using InstanceInfo = typename Base::InstanceInfo;

  friend class faithful::ShaderProgram;

  ShaderProgramManager();
  ~ShaderProgramManager();

  /// not copyable
  ShaderProgramManager(const ShaderProgramManager&) = delete;
  ShaderProgramManager& operator=(const ShaderProgramManager&) = delete;

  /// movable
  ShaderProgramManager(ShaderProgramManager&&) = default;
  ShaderProgramManager& operator=(ShaderProgramManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& texture_path);


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


 protected:
  bool IsValidShader(GLuint shader, GLenum shader_type) noexcept;
  bool IsValidProgram() noexcept;

  void AttachShader(const char* path, GLenum shader_type) noexcept;

  bool ReadToBuffer(const char* path) noexcept;

 private:
  char* buffer_ = nullptr;
  GLint buffer_size_ = 0;
  GLint success_ = 0;
  GLuint program_ = 0;
  int default_shader_program_id_ = 0; // adjust
};

} // namespace shader
} // namespace details

// TODO: inherit from details::IResource

class ShaderProgram {
 public:
  ShaderProgram() = delete;
  ShaderProgram(details::shader::ShaderProgramManager* manager) : manager_(manager) {
    id_ = 0; /// id == 0 - id for default texture
  }

  ShaderProgram(const ShaderProgram& other) {
    manager_->ReuseShaderProgram(other.id_);
  }
  ShaderProgram(ShaderProgram&& other) {
    other.id_ = 0;
    manager_->ReuseShaderProgram(other.id_);
  }

  ShaderProgram& operator=(const ShaderProgram& other) {
    if (other == *this) {
      return *this;
    }
    RestoreTextureId();
    manager_->ReuseShaderProgram(other.id_);
    return *this;
  }
  ShaderProgram& operator=(ShaderProgram&& other) {
    if (other == *this) {
      return *this;
    }
    RestoreTextureId();
    other.id_ = 0;
    manager_->ReuseShaderProgram(other.id_);
    return *this;
  }

  ~ShaderProgram() {
    RestoreTextureId();
  }

  void Bake();

  void AttachShader(GLenum shader_type, const ShaderObject& shader_obj);

  void Use();

  friend bool operator==(const ShaderProgram& tex1, const ShaderProgram& tex2) {
    /// requires the same instance (not content equality)
    return tex1.id_ == tex2.id_ && &*tex1.manager_ == &*tex2.manager_;
  }

 private:
  void RestoreTextureId() {
    if (id_ != 0) {
      manager_->Restore(id_);
    }
  }
  details::shader::ShaderProgramManager* manager_;
  int id_;
  /// if baked we can't attach shader objects
  bool baked_ = false;
};

}  // namespace faithful

#include "ShaderProgram-inl.h"

#endif  // FAITHFUL_SHADERPROGRAM_H
