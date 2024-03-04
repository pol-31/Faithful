#include "ShaderProgramData.h"

#include "../../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {
namespace assets {

ShaderProgramData::~ShaderProgramData() {
  if (opengl_context) {
    opengl_context->Put([=]{
      glDeleteProgram(id);
    });
  }
}

} // namespace assets
} // namespace details
} // namespace faithful
