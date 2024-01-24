#ifndef FAITHFUL_SHADERPROGRAM_INL_H
#define FAITHFUL_SHADERPROGRAM_INL_H

#include "ShaderProgram.h"

namespace faithful {
namespace utility {

inline void ShaderManager::SetUniform(
    const GLchar* name, GLboolean v0) {
  glUniform1f(glGetUniformLocation(program_, name), static_cast<int>(v0));
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLfloat v0) {
  glUniform1f(glGetUniformLocation(program_, name), v0);
}

inline void ShaderManager::SetUniform(
    const GLchar* name, GLfloat v0, GLfloat v1) {
  glUniform2f(glGetUniformLocation(program_, name), v0, v1);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2) {
  glUniform3f(glGetUniformLocation(program_, name), v0, v1, v2);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  glUniform4f(glGetUniformLocation(program_, name), v0, v1, v2, v3);
}

inline void ShaderManager::SetUniform(
    const GLchar* name, GLint v0) {
  glUniform1i(glGetUniformLocation(program_, name), v0);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLint v0, GLint v1) {
  glUniform2i(glGetUniformLocation(program_, name), v0, v1);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLint v0, GLint v1, GLint v2) {
  glUniform3i(glGetUniformLocation(program_, name), v0, v1, v2);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLint v0, GLint v1, GLint v2, GLint v3) {
  glUniform4i(glGetUniformLocation(program_, name), v0, v1, v2, v3);
}

inline void ShaderManager::SetUniform(
    const GLchar* name, GLuint v0) {
  glUniform1ui(glGetUniformLocation(program_, name), v0);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLuint v0, GLuint v1) {
  glUniform2ui(glGetUniformLocation(program_, name), v0, v1);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLuint v0, GLuint v1, GLuint v2) {
  glUniform3ui(glGetUniformLocation(program_, name), v0, v1, v2);
}
inline void ShaderManager::SetUniform(
    const GLchar* name, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
  glUniform4ui(glGetUniformLocation(program_, name), v0, v1, v2, v3);
}

inline void ShaderManager::SetUniformVec1(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform1fv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec2(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform2fv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec3(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform3fv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec4(
    const GLchar* name, GLsizei count, const GLfloat* value) {
  glUniform4fv(glGetUniformLocation(program_, name), count, value);
}

inline void ShaderManager::SetUniformVec1(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform1iv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec2(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform2iv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec3(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform3iv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec4(
    const GLchar* name, GLsizei count, const GLint* value) {
  glUniform4iv(glGetUniformLocation(program_, name), count, value);
}

inline void ShaderManager::SetUniformVec1(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform1uiv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec2(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform2uiv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec3(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform3uiv(glGetUniformLocation(program_, name), count, value);
}
inline void ShaderManager::SetUniformVec4(
    const GLchar* name, GLsizei count, const GLuint* value) {
  glUniform4uiv(glGetUniformLocation(program_, name), count, value);
}

inline void ShaderManager::SetUniformMat2v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix2fv(glGetUniformLocation(program_, name),
                     count, transpose, value);
}
inline void ShaderManager::SetUniformMat3v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix3fv(glGetUniformLocation(program_, name),
                     count, transpose, value);
}
inline void ShaderManager::SetUniformMat4v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix4fv(glGetUniformLocation(program_, name),
                     count, transpose, value);
}
inline void ShaderManager::SetUniformMat2x3v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix2x3fv(glGetUniformLocation(program_, name),
                       count, transpose, value);
}
inline void ShaderManager::SetUniformMat3x2v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix3x2fv(glGetUniformLocation(program_, name),
                       count, transpose, value);
}
inline void ShaderManager::SetUniformMat2x4v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix2x4fv(glGetUniformLocation(program_, name),
                       count, transpose, value);
}
inline void ShaderManager::SetUniformMat4x2v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix4x2fv(glGetUniformLocation(program_, name),
                       count, transpose, value);
}
inline void ShaderManager::SetUniformMat3x4v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix3x4fv(glGetUniformLocation(program_, name),
                       count, transpose, value);
}
inline void ShaderManager::SetUniformMat4x3v(
    const GLchar* name, GLsizei count,
    GLboolean transpose, const GLfloat* value) {
  glUniformMatrix4x3fv(glGetUniformLocation(program_, name),
                       count, transpose, value);
}

}  // namespace utility
}  // namespace faithful

#endif  // FAITHFUL_SHADERPROGRAM_INL_H
