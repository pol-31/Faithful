#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADERPROGRAMDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADERPROGRAMDATA_H_

#include <glad/glad.h>
#include "../ShaderProgramShaders.h"

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace assets {

struct ShaderProgramData {
  GLuint id;
  ProgramShaders shaders;
  bool baked = false;
  bool ready = false;
  DisplayInteractionThreadPool* opengl_context = nullptr;

  ~ShaderProgramData();
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_SHADERPROGRAMDATA_H_
