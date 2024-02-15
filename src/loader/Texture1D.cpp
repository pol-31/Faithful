#include "Texture1D.h"

#include <glad/glad.h>

namespace faithful {

void Texture1D::Bind() {
  glBindTexture(GL_TEXTURE_1D, internal_id_);
}

} // namespace faithful
