#ifndef FAITHFUL_SRC_LOADER_MODEL_LOAD_H_
#define FAITHFUL_SRC_LOADER_MODEL_LOAD_H_

#include <iostream>

#include <glad/glad.h>
#include "glm/gtc/matrix_transform.hpp"
#include <tiny_gltf.h>

#include "../../common/Object.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace faithful {
namespace details {
namespace assets {

class CommonModelLoader {
 public:
  void Load();

 private:
  bool LoadModel(tinygltf::Model &model, const char *filename);
  void BindMesh(std::map<int, GLuint>& vbos,
                tinygltf::Model &model, tinygltf::Mesh &mesh);
  void BindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model,
                      tinygltf::Node &node);
  std::pair<GLuint, std::map<int, GLuint>> BindModel(tinygltf::Model &model);
  void DbgModel(tinygltf::Model &model);
};

class CommonModelDrawer : Drawable {
 public:
  bool Draw() override;

 private:
  void DrawMesh(const std::map<int, GLuint>& vbos,
                tinygltf::Model &model, tinygltf::Mesh &mesh);
  void DrawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos,
                      tinygltf::Model &model, tinygltf::Node &node);
  void DrawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos,
                 tinygltf::Model &model);
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_LOAD_H_
