#ifndef FAITHFUL_SRC_LOADER_SHADERPROGRAMSHADERS_H_
#define FAITHFUL_SRC_LOADER_SHADERPROGRAMSHADERS_H_

#include "ShaderObject.h"

namespace faithful {
namespace details {
namespace assets {

struct ProgramShaders {
  ShaderObject vertex;
  ShaderObject fragment;
  ShaderObject geometry;
  ShaderObject tessellation_control;
  ShaderObject tessellation_evaluation;
  ShaderObject compute;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADERPROGRAMSHADERS_H_
