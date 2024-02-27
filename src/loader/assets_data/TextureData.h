#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_TEXTUREDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_TEXTUREDATA_H_

#include <glad/glad.h>

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace assets {

struct TextureData {
  GLuint id = 0;
  bool ready = false;
  DisplayInteractionThreadPool* opengl_context = nullptr;

  // TODO: the same for shader object / shader program / model
  ~TextureData() {
    if (opengl_context) {
      opengl_context->Put([=]{
        glDeleteTextures(1, &id);
      });
    }
  }
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_TEXTUREDATA_H_
