#include "Texture2D.h"

#include <glad/glad.h>

namespace faithful {

void Texture2D::Bind() {
  glBindTexture(GL_TEXTURE_2D, internal_id_);
}

} // namespace faithful
