#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADERPROGRAMDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADERPROGRAMDATA_H_

#include <glad/glad.h>
#include "../ShaderProgramShaders.h"

namespace faithful {
namespace details {
namespace assets {

struct ShaderObjectData {
  GLint id;
  ProgramShaders shaders_;
  bool baked_ = false;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADERPROGRAMDATA_H_
