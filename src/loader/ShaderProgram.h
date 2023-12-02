#ifndef FAITHFUL_SHADERPROGRAM_H
#define FAITHFUL_SHADERPROGRAM_H

#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <iostream>

namespace faithful {

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/*

namespace utility {

// TODO: rename include-guard macros and add to namespace __simurgh__

// TODO: logging system + macros _IS_DEBUG_MODE_ and full logging
// TODO: buffer system
// TODO: explanation (class, buffer_)

// Class ShaderProgram stands for generating one specific OpenGl shader program
// using vertex, fragment and geometry shaders, which have to be passed to ctor
// through their paths.
// Only movable, non-copyable.
class ShaderProgram {
 public:

  ShaderProgram();


  ShaderProgram(
    const char *vertex_shader_path,
    const char *fragment_shader_path,
    const char *geometry_shader_path = nullptr) noexcept;

  ShaderProgram(const ShaderProgram &) = delete;

  ShaderProgram(ShaderProgram &&sp) noexcept;

  ShaderProgram &operator=(const ShaderProgram &) = delete;

  ShaderProgram &operator=(ShaderProgram &&sp) noexcept;

  // operator= (paths...)

  ~ShaderProgram();

  void UseProgram() const noexcept {
    if (program_)
      glUseProgram(program_);
  }

  GLuint get_program() const noexcept {
    return program_;
  };

  const char *get_buffer() const noexcept {
    return buffer_;
  }

  char *get_buffer() noexcept {
    return buffer_;
  }

  void set_buffer(char *buffer) noexcept;

  inline void AdjustVar(const GLchar *name, GLboolean v0);
  inline void AdjustVar(const GLchar *name, GLfloat v0);
  inline void AdjustVar(const GLchar *name, GLfloat v0, GLfloat v1);
  inline void AdjustVar(const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2);
  inline void AdjustVar(const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
  inline void AdjustVar(const GLchar *name, GLint v0);
  inline void AdjustVar(const GLchar *name, GLint v0, GLint v1);
  inline void AdjustVar(const GLchar *name, GLint v0, GLint v1, GLint v2);
  inline void AdjustVar(const GLchar *name, GLint v0, GLint v1, GLint v2, GLint v3);
  inline void AdjustVar(const GLchar *name, GLuint v0);
  inline void AdjustVar(const GLchar *name, GLuint v0, GLuint v1);
  inline void AdjustVar(const GLchar *name, GLuint v0, GLuint v1, GLuint v2);
  inline void AdjustVar(const GLchar *name, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
  inline void AdjustVec1(const GLchar *name, GLsizei count, const GLfloat *value);
  inline void AdjustVec2(const GLchar *name, GLsizei count, const GLfloat *value);
  inline void AdjustVec3(const GLchar *name, GLsizei count, const GLfloat *value);
  inline void AdjustVec4(const GLchar *name, GLsizei count, const GLfloat *value);
  inline void AdjustVec1(const GLchar *name, GLsizei count, const GLint *value);
  inline void AdjustVec2(const GLchar *name, GLsizei count, const GLint *value);
  inline void AdjustVec3(const GLchar *name, GLsizei count, const GLint *value);
  inline void AdjustVec4(const GLchar *name, GLsizei count, const GLint *value);
  inline void AdjustVec1(const GLchar *name, GLsizei count, const GLuint *value);
  inline void AdjustVec2(const GLchar *name, GLsizei count, const GLuint *value);
  inline void AdjustVec3(const GLchar *name, GLsizei count, const GLuint *value);
  inline void AdjustVec4(const GLchar *name, GLsizei count, const GLuint *value);
  inline void AdjustMat2v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat3v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat4v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat2x3v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat3x2v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat2x4v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat4x2v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat3x4v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);
  inline void AdjustMat4x3v(const GLchar *name, GLsizei count, GLboolean transpose, const GLfloat *value);

 protected:
  friend class DefaultShaderProgram;

  void set_program(GLuint program) {
    program_ = program;
  }

 private:
  bool IsValidShader(GLuint shader, GLenum shader_type) noexcept;
  bool IsValidProgram() noexcept;
  void AttachShader(const char *path, GLenum shader_type) noexcept;
  bool ReadToBuffer(const char *path) noexcept;

  char *buffer_ = nullptr;
  GLint buffer_size_ = 0;
  GLint success_ = 0;
  GLuint program_ = 0;
};

class DefaultShaderProgram {
 public:
  static void Init();
  static ShaderProgram* cubemap_;
  static ShaderProgram* object2d_;
  static ShaderProgram* object3d_;
  static ShaderProgram* object3d_light_;
  static ShaderProgram* object3d_light_parallax_;
  static ShaderProgram* object3d_skinned_;
  static ShaderProgram* object3d_skinned_light_;
  static ShaderProgram* object3d_skinned_light_parallax_;
  // TODO: different amount of bones
  // TODO: light_emitter_shader_program_;

  static ShaderProgram* picking_object_shader_program_; // the same for 2d and 3d // TODO: rename
  static ShaderProgram* hud_stencil_shader_program_; // useful optimization (can be used with GL_BLEND)
  // TODO: bloom, tbn, ssao...

  static ShaderProgram* TEST_copy_bone_shader();

  static ShaderProgram* CreateBoneProgram();
//  static ShaderProgram* CreateBoneLightProgram();
//  static ShaderProgram* CreateBoneLightParallaxProgram();


  static unsigned int bone_vectex_shader_;
  static unsigned int bone_light_vectex_shader_;
  static unsigned int bone_light_parallax_vectex_shader_;

  static unsigned int bone_fragment_shader_;
  static unsigned int bone_light_fragment_shader_;
  static unsigned int bone_light_parallax_fragment_shader_;

  static char* object_vertex_shader_path_;
  static char* object_fragment_shader_path_;
  static char* cubemap_vertex_shader_path_;
  static char* cubemap_fragment_shader_path_;
  static char* hud_vertex_shader_path_;
  static char* hud_fragment_shader_path_;
  static char* picking_object_vertex_shader_path_;
  static char* picking_object_fragment_shader_path_;
  static char* hud_stencil_vertex_shader_path_;
  static char* hud_stencil_fragment_shader_path_;
  static char* bone_3d_vertex_shader_path_;
  static char* bone_3d_fragment_shader_path_;

  static GLuint ubo_matrices_;

 private:
  static bool IsValidShader(GLuint shader, GLenum shader_type) noexcept;
  static bool IsValidProgram() noexcept;
  static void AttachShader(const char *path, GLenum shader_type) noexcept;
  static bool ReadToBuffer(const char *path) noexcept;

  static bool initialized_;

  static char *buffer_;
  static GLint buffer_size_;
  static GLint success_;
  static GLuint program_;
};

} // namespace utility
*/
} // namespace faithful

#include "ShaderProgram-inl.h"

#endif // FAITHFUL_SHADERPROGRAM_H

