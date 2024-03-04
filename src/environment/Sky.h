#ifndef FAITHFUL_SRC_ENVIRONMENT_SKY_H_
#define FAITHFUL_SRC_ENVIRONMENT_SKY_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../loader/ShaderProgram.h"
#include "../loader/Texture.h"

namespace faithful {

// TODO: add Anisotropic filtering to hdr texture

struct Sky {
  Texture texture;
  // TODO: Texture ibl_texture;
  glm::vec3 color;
};

class SkyHandler {
 public:
  SkyHandler() = delete;
  SkyHandler(ShaderProgram shader_program)
      : shader_program_(shader_program) {}

  void SetSky(Sky terrain) {
    terrain_ = terrain;
  }

  // should be called from OpenGL thread just before drawing
  void Bind() {
    shader_program_.Bind();
    shader_program_.SetUniformVec3("color", 1, glm::value_ptr(terrain_.color));
    glActiveTexture(GL_TEXTURE0);
    terrain_.texture.Bind();
  }

 private:
  ShaderProgram shader_program_;
  Sky terrain_;
};


} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_SKY_H_
