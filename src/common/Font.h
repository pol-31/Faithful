#ifndef FAITHFUL_SRC_COMMON_FONT_H_
#define FAITHFUL_SRC_COMMON_FONT_H_

#include <glad/glad.h>

namespace faithful {
namespace details {

struct Font {
  const GLenum texture_id; // e.g. GL_TEXTURE11
  const int tex_width;
  const int tex_height;
  const int cell_height;
  const int cell_width;
  const int font_height;
  const int font_width;
  const int symbol_width[96];
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_FONT_H_
