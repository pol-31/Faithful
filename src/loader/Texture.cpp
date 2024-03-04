#include "Texture.h"

#include <glad/glad.h>

namespace faithful {

void Texture::Bind() {
  glBindTexture(GL_TEXTURE_2D, data_->id);
}

} // namespace faithful
