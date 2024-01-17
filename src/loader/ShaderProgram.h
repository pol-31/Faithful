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

// Class ShaderProgram stands for generating one specific OpenGl shader program
// using vertex, fragment and geometry shaders, which have to be passed to ctor
// through their paths.
// Only movable, non-copyable.
class ShaderManager {
 public:
  ShaderManager() = default;
  static bool Init() {
    // shader creation
    return true;
  }

 private:
  using Shader = int;
  static Shader shader1;
  static Shader shader2;
  static Shader shader3;
  // ... etc

  static char* path1 /* = "path1"*/;
  static char* path2 /* = "path2"*/;
  // ... etc
};

class ShaderProgram {
 public:
  ShaderProgram();

  ShaderProgram(const char* vertex_shader_path,
                const char* fragment_shader_path,
                const char* geometry_shader_path = nullptr) noexcept;

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&& sp) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&& sp) = delete;

  ~ShaderProgram();

  void Use() const noexcept {
    if (program_) {
      glUseProgram(program_);
    }
  }

  GLuint Id() const noexcept {
    return program_;
  };

  inline void AdjustVar(const GLchar* name, GLboolean v0);
  inline void AdjustVar(const GLchar* name, GLfloat v0);
  inline void AdjustVar(const GLchar* name, GLfloat v0, GLfloat v1);
  inline void AdjustVar(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2);

  inline void AdjustVar(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2,
                        GLfloat v3);

  inline void AdjustVar(const GLchar* name, GLint v0);

  inline void AdjustVar(const GLchar* name, GLint v0, GLint v1);

  inline void AdjustVar(const GLchar* name, GLint v0, GLint v1, GLint v2);

  inline void AdjustVar(const GLchar* name, GLint v0, GLint v1, GLint v2,
                        GLint v3);

  inline void AdjustVar(const GLchar* name, GLuint v0);

  inline void AdjustVar(const GLchar* name, GLuint v0, GLuint v1);

  inline void AdjustVar(const GLchar* name, GLuint v0, GLuint v1, GLuint v2);

  inline void AdjustVar(const GLchar* name, GLuint v0, GLuint v1, GLuint v2,
                        GLuint v3);

  inline void AdjustVec1(const GLchar* name, GLsizei count,
                         const GLfloat* value);

  inline void AdjustVec2(const GLchar* name, GLsizei count,
                         const GLfloat* value);

  inline void AdjustVec3(const GLchar* name, GLsizei count,
                         const GLfloat* value);

  inline void AdjustVec4(const GLchar* name, GLsizei count,
                         const GLfloat* value);

  inline void AdjustVec1(const GLchar* name, GLsizei count, const GLint* value);

  inline void AdjustVec2(const GLchar* name, GLsizei count, const GLint* value);

  inline void AdjustVec3(const GLchar* name, GLsizei count, const GLint* value);

  inline void AdjustVec4(const GLchar* name, GLsizei count, const GLint* value);

  inline void AdjustVec1(const GLchar* name, GLsizei count,
                         const GLuint* value);

  inline void AdjustVec2(const GLchar* name, GLsizei count,
                         const GLuint* value);

  inline void AdjustVec3(const GLchar* name, GLsizei count,
                         const GLuint* value);

  inline void AdjustVec4(const GLchar* name, GLsizei count,
                         const GLuint* value);

  inline void AdjustMat2v(const GLchar* name, GLsizei count,
                          GLboolean transpose, const GLfloat* value);

  inline void AdjustMat3v(const GLchar* name, GLsizei count,
                          GLboolean transpose, const GLfloat* value);

  inline void AdjustMat4v(const GLchar* name, GLsizei count,
                          GLboolean transpose, const GLfloat* value);

  inline void AdjustMat2x3v(const GLchar* name, GLsizei count,
                            GLboolean transpose, const GLfloat* value);

  inline void AdjustMat3x2v(const GLchar* name, GLsizei count,
                            GLboolean transpose, const GLfloat* value);

  inline void AdjustMat2x4v(const GLchar* name, GLsizei count,
                            GLboolean transpose, const GLfloat* value);

  inline void AdjustMat4x2v(const GLchar* name, GLsizei count,
                            GLboolean transpose, const GLfloat* value);

  inline void AdjustMat3x4v(const GLchar* name, GLsizei count,
                            GLboolean transpose, const GLfloat* value);

  inline void AdjustMat4x3v(const GLchar* name, GLsizei count,
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

}  // namespace utility
}  // namespace faithful

#include "ShaderProgram-inl.h"

#endif  // FAITHFUL_SHADERPROGRAM_H
