#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_MODELDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_MODELDATA_H_

#include <vector>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "../../collision/CollisionArea.h"
#include "../../common/Material.h"
#include "../Sound.h"

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace assets {

struct ModelData {
  bool ready = false;
  int global_model_id;

  GLuint vao;
  std::map<int, GLuint> vbo_and_ebos;

  tinygltf::Model model;
  Material material;
  CollisionArea collision_area;

  std::vector<Sound> sounds;

  DisplayInteractionThreadPool* opengl_context = nullptr;

  ~ModelData();
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_MODELDATA_H_
