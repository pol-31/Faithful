#ifndef FAITHFUL_SRC_ENVIRONMENT_TERRAIN_H_
#define FAITHFUL_SRC_ENVIRONMENT_TERRAIN_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../loader/ShaderProgram.h"
#include "../loader/Texture.h"

namespace faithful {

struct Terrain {
  Texture texture;
  glm::vec3 color;
};

class TerrainHandler {
 public:
  TerrainHandler() = delete;
  TerrainHandler(ShaderProgram shader_program)
      : shader_program_(shader_program) {}

  void SetTerrain(Terrain terrain) {
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
  Terrain terrain_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_TERRAIN_H_
