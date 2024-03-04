#include "ShaderObjectData.h"

#include "../../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {
namespace assets {

ShaderObjectData::~ShaderObjectData() {
  if (opengl_context) {
    opengl_context->Put([=]{
      glDeleteShader(id);
    });
  }
}

} // namespace assets
} // namespace details
} // namespace faithful
