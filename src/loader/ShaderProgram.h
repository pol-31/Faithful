#ifndef FAITHFUL_SRC_LOADER_SHADERPROGRAM_H_
#define FAITHFUL_SRC_LOADER_SHADERPROGRAM_H_

#include "AssetBase.h"
#include "assets_data/ShaderProgramData.h"

#include "../common/OpenGlContextAwareBase.h"

namespace faithful {
namespace details {
namespace assets {

class ShaderObjectPool;

} // namespace assets
} // namespace details

class ShaderProgram
    : public details::assets::AssetBase<details::assets::ShaderProgramData>,
      public details::assets::OpenGlContextAwareBase {
 public:
  using Base = details::assets::AssetBase<details::assets::ShaderProgramData>;
  using Base::Base;
  using Base::operator=;

  GLuint GetId() const {
    return data_->id;
  }

  bool Ready() const {
    return data_->ready;
  }

  bool Baked() const {
    return data_->baked;
  }

  details::assets::ProgramShaders GetProgramShaders() const {
    return data_->shaders;
  }

  void SetShaderObjectPool(details::assets::ShaderObjectPool*
                               shader_object_pool);

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

  static void CheckValidity(GLuint id);

 private:
  details::assets::ShaderObjectPool* shader_object_pool_;
  using Base::data_;
};

}  // namespace faithful

#include "ShaderProgram-inl.h"

#endif  // FAITHFUL_SRC_LOADER_SHADERPROGRAM_H_
