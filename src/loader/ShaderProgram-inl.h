#ifndef FAITHFUL_SRC_LOADER_SHADERPROGRAM_INL_H_
#define FAITHFUL_SRC_LOADER_SHADERPROGRAM_INL_H_

#include "ShaderProgram.h"

namespace faithful {

inline void ShaderProgram::SetUniform(
    const GLchar* name, GLboolean v0) {
  glUniform1f(glGetUniformLocation(data_->id, name), static_cast<int>(v0));
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLfloat v0) {
  glUniform1f(glGetUniformLocation(data_->id, name), v0);
}

inline void ShaderProgram::SetUniform(
    const GLchar* name, GLfloat v0, GLfloat v1) {
  glUniform2f(glGetUniformLocation(data_->id, name), v0, v1);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2) {
  glUniform3f(glGetUniformLocation(data_->id, name), v0, v1, v2);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  glUniform4f(glGetUniformLocation(data_->id, name), v0, v1, v2, v3);
}

inline void ShaderProgram::SetUniform(
    const GLchar* name, GLint v0) {
  glUniform1i(glGetUniformLocation(data_->id, name), v0);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLint v0, GLint v1) {
  glUniform2i(glGetUniformLocation(data_->id, name), v0, v1);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLint v0, GLint v1, GLint v2) {
  glUniform3i(glGetUniformLocation(data_->id, name), v0, v1, v2);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLint v0, GLint v1, GLint v2, GLint v3) {
  glUniform4i(glGetUniformLocation(data_->id, name), v0, v1, v2, v3);
}

inline void ShaderProgram::SetUniform(
    const GLchar* name, GLuint v0) {
  glUniform1ui(glGetUniformLocation(data_->id, name), v0);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLuint v0, GLuint v1) {
  glUniform2ui(glGetUniformLocation(data_->id, name), v0, v1);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLuint v0, GLuint v1, GLuint v2) {
  glUniform3ui(glGetUniformLocation(data_->id, name), v0, v1, v2);
}
inline void ShaderProgram::SetUniform(
    const GLchar* name, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
  glUniform4ui(glGetUniformLocation(data_->id, name), v0, v1, v2, v3);
}

inline void ShaderProgram::SetUniformVec1(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform1fv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec2(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform2fv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec3(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform3fv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec4(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform4fv(glGetUniformLocation(data_->id, name), count, value);
}

inline void ShaderProgram::SetUniformVec1(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform1iv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec2(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform2iv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec3(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform3iv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec4(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform4iv(glGetUniformLocation(data_->id, name), count, value);
}

inline void ShaderProgram::SetUniformVec1(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform1uiv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec2(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform2uiv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec3(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform3uiv(glGetUniformLocation(data_->id, name), count, value);
}
inline void ShaderProgram::SetUniformVec4(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform4uiv(glGetUniformLocation(data_->id, name), count, value);
}

inline void ShaderProgram::SetUniformMat2v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix2fv(glGetUniformLocation(data_->id, name),
                     count, transpose, value);
}
inline void ShaderProgram::SetUniformMat3v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix3fv(glGetUniformLocation(data_->id, name),
                     count, transpose, value);
}
inline void ShaderProgram::SetUniformMat4v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix4fv(glGetUniformLocation(data_->id, name),
                     count, transpose, value);
}
inline void ShaderProgram::SetUniformMat2x3v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix2x3fv(glGetUniformLocation(data_->id, name),
                       count, transpose, value);
}
inline void ShaderProgram::SetUniformMat3x2v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix3x2fv(glGetUniformLocation(data_->id, name),
                       count, transpose, value);
}
inline void ShaderProgram::SetUniformMat2x4v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix2x4fv(glGetUniformLocation(data_->id, name),
                       count, transpose, value);
}
inline void ShaderProgram::SetUniformMat4x2v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix4x2fv(glGetUniformLocation(data_->id, name),
                       count, transpose, value);
}
inline void ShaderProgram::SetUniformMat3x4v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix3x4fv(glGetUniformLocation(data_->id, name),
                       count, transpose, value);
}
inline void ShaderProgram::SetUniformMat4x3v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix4x3fv(glGetUniformLocation(data_->id, name),
                       count, transpose, value);
}

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADERPROGRAM_INL_H_
