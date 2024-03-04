#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADEROBJECTDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADEROBJECTDATA_H_

#include <glad/glad.h>

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace assets {

struct ShaderObjectData {
  GLenum type;
  GLuint id = 0;
  bool ready = false;
  DisplayInteractionThreadPool* opengl_context = nullptr;

  ~ShaderObjectData();
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADEROBJECTDATA_H_
