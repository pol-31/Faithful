#ifndef FAITHFUL_SRC_LOADER_OPENGLCONTEXTAWAREBASE_H_
#define FAITHFUL_SRC_LOADER_OPENGLCONTEXTAWAREBASE_H_

#include "../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {

class DisplayInteractionThreadPool;

namespace assets {

class OpenGlContextAwareBase {
 public:
  void SetOpenGlContext(DisplayInteractionThreadPool* opengl_context) {
    opengl_context_ = opengl_context;
  }

 protected:
  DisplayInteractionThreadPool* opengl_context_;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_OPENGLCONTEXTAWAREBASE_H_
