#include "TextureData.h"

#include "../../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {
namespace assets {

TextureData::~TextureData() {
  if (opengl_context) {
    opengl_context->Put([=]{
      glDeleteTextures(1, &id);
    });
  }
}

} // namespace assets
} // namespace details
} // namespace faithful
