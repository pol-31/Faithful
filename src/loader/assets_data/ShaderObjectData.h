#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADEROBJECTDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADEROBJECTDATA_H_

#include <glad/glad.h>

namespace faithful {
namespace details {
namespace assets {

struct ShaderObjectData {
  GLint id;
  GLenum type;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADEROBJECTDATA_H_
