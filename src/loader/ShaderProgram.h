#ifndef FAITHFUL_SHADERPROGRAM_H
#define FAITHFUL_SHADERPROGRAM_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <iostream>


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
namespace utility {

/// should be only 1 instance for the entire program
class ShaderManager {
 public:
  ShaderManager();

  ShaderManager(const char* vertex_shader_path,
                const char* fragment_shader_path,
                const char* geometry_shader_path = nullptr) noexcept;

  ShaderManager(const ShaderManager&) = delete;
  ShaderManager(ShaderManager&& sp) = delete;
  ShaderManager& operator=(const ShaderManager&) = delete;
  ShaderManager& operator=(ShaderManager&& sp) = delete;

  ~ShaderManager();

  void ReuseShaderProgram(int opengl_id);
  void Restore(int opengl_id);

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
  friend class DefaultShaderProgram;

  void SetProgram(GLuint program) {
    program_ = program;
  }

 private:
  bool IsValidShader(GLuint shader, GLenum shader_type) noexcept;

  bool IsValidProgram() noexcept;

  void AttachShader(const char* path, GLenum shader_type) noexcept;

  bool ReadToBuffer(const char* path) noexcept;

  char* buffer_ = nullptr;
  GLint buffer_size_ = 0;
  GLint success_ = 0;
  GLuint program_ = 0;
};

class ShaderObject {
 public:
  ShaderObject(GLenum shader_type, std::string&& path) {
    // load
    // compile
    // keep id_
  }

  /* typical scenario:
   * { // one scope
   *  ShaderObject sho1(vertex_type, "some path 1");
   *  ShaderObject sho2(fragment_type, "some path 2");
   *
   *
   *  // create program_1
   *  program_1.AttachShader(sho1, sho2);
   *
   *
   *  // there it has not been deleted and internally it reused
   *  // but ++ref_counter
   *  ShaderObject sho3(vertex_type, "some path 1");
   *
   * program_2.AttachShader(sho3); <-- successfully reused
   *
   * }
   *
   * */

  ~ShaderObject() {
    if (id_ != 0) {
      // glDeleteShader();
    }
  }

  void Load(std::string&& path);

 private:
  ShaderManager* manager_;
  int id_ = 0;
};

class ShaderProgram {
 public:
  ShaderProgram() = delete;
  ShaderProgram(ShaderManager* manager) : manager_(manager) {
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
  ShaderManager* manager_;
  int id_;
  /// if baked we can't attach shader objects
  bool baked_ = false;
};

}  // namespace utility
}  // namespace faithful

#include "ShaderProgram-inl.h"

#endif  // FAITHFUL_SHADERPROGRAM_H
