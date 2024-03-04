#ifndef FAITHFUL_SRC_LOADER_MODEL_NONANIMATABLEMODEL_H_
#define FAITHFUL_SRC_LOADER_MODEL_NONANIMATABLEMODEL_H_

#include <glm/glm.hpp>

#include "ModelBase.h"
#include "../ShaderProgram.h"

namespace faithful {

class NonAnimatableModel : public ModelBase {
 public:
  using Base = ModelBase;
  using Base::Base;
  using Base::operator=;

  void Draw(const std::vector<glm::mat4>& transforms,
            ShaderProgram& shader_program);

 private:
  void DrawMeshes(const std::vector<glm::mat4>& transforms,
                  ShaderProgram& shader_program,
                  tinygltf::Mesh &mesh);

  void DrawModelNodes(const std::vector<glm::mat4>& transforms,
                      ShaderProgram& shader_program,
                      tinygltf::Node &node);

  using Base::data_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_NONANIMATABLEMODEL_H_
