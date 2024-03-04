#include "ModelData.h"

#include "../../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {
namespace assets {

ModelData::~ModelData() {
  if (opengl_context) {
    opengl_context->Put([=]{
      glDeleteVertexArrays(1, &vao);
    });
  }
}

} // namespace assets
} // namespace details
} // namespace faithful
