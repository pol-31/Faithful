#ifndef FAITHFUL_SRC_LOADER_SHADERPROGRAM_H_
#define FAITHFUL_SRC_LOADER_SHADERPROGRAM_H_

#include "glad/glad.h"

#include "IAsset.h"
#include "../config/Loader.h"

#include "ShaderObject.h"

namespace faithful {

namespace details {
namespace assets {

class ShaderPool;

} // namespace assets
} // namespace details

class ShaderProgram : public details::assets::IAsset {
 public:
  using Base = details::assets::IAsset;
  using Base::Base;
  using Base::operator=;

  ShaderProgram(details::RefCounter* ref_counter, GLuint id,
                details::assets::ShaderPool* shader_manager);

  void Bind();

  void AttachShader(const ShaderObject& shader_obj);
  void DetachShader(const ShaderObject& shader_obj);

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
  struct ProgramShaders {
    ShaderObject vertex;
    ShaderObject fragment;
    ShaderObject geometry;
    ShaderObject tessellation_control;
    ShaderObject tessellation_evaluation;
    ShaderObject compute;
  };

  bool IsValidProgram() noexcept;

  using Base::internal_id_;

  details::assets::ShaderPool* shader_manager_;

  /// there is possible only 6 types of shader by now
  /// they comes in such order: vert, frag, geom, tess_control, tess_eval, comp
  ProgramShaders shaders_;

  /// if baked we can't attach shader objects
  bool baked_ = false;
};

}  // namespace faithful

#include "Shader-inl.h"

#endif  // FAITHFUL_SRC_LOADER_SHADERPROGRAM_H_
