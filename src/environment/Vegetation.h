#ifndef FAITHFUL_SRC_ENVIRONMENT_GRASS_H_
#define FAITHFUL_SRC_ENVIRONMENT_GRASS_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../loader/ShaderProgram.h"
#include "../loader/Texture.h"

namespace faithful {

struct Vegetation {
  struct Grass {
    glm::vec3 color;
    float grass_bending;
    float grass_long_height;
    float grass_short_height;
    float grass_density;
    Texture texture;
  };
  struct Leafs {
    glm::vec3 color;
    float leafs_size;
    float leafs_density;
    Texture texture;
  };

  Grass grass;
  Leafs leafs;
};

class VegetationHandler {
 public:
  VegetationHandler() = delete;
  VegetationHandler(ShaderProgram grass_shader_program,
                    ShaderProgram leafs_shader_program)
      : grass_shader_program_(grass_shader_program),
        leafs_shader_program_(leafs_shader_program) {}

  void SetVegetation(Vegetation vegetation) {
    vegetation_ = vegetation;
  }

  // All should be called from OpenGL thread just before drawing
  void BindLeafs() {
    leafs_shader_program_.Bind();
    leafs_shader_program_.SetUniformVec3("color", 1, glm::value_ptr(vegetation_.grass.color));
    glActiveTexture(GL_TEXTURE0);
    vegetation_.leafs.texture.Bind();
  }
  void BindLongGrass() {
    grass_shader_program_.Bind();
    grass_shader_program_.SetUniformVec3("color", 1, glm::value_ptr(vegetation_.grass.color));
    glActiveTexture(GL_TEXTURE0);
    vegetation_.grass.texture.Bind();

  }
  void BindShortGrass() {
    grass_shader_program_.Bind();
    grass_shader_program_.SetUniformVec3("color", 1, glm::value_ptr(vegetation_.leafs.color));
    glActiveTexture(GL_TEXTURE0);
    vegetation_.grass.texture.Bind();
  }

 private:
  ShaderProgram grass_shader_program_;
  ShaderProgram leafs_shader_program_;
  Vegetation vegetation_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_GRASS_H_
