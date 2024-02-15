#ifndef FAITHFUL_SRC_LOADER_ANIMATION_H_
#define FAITHFUL_SRC_LOADER_ANIMATION_H_

#include "glm/glm/glm.hpp"

#include "../../utils/Span.h"

namespace faithful {

class Animation {
 public:
  Animation() = default;
  Animation(void* animation_data) : animation_data_(animation_data) {}

  void Run(int animation_id);
  void Stop();

  void SetCurrent(int animation_id);
  void UnsetCurrent();

  void SetNext(int animation_id);
  void UnsetNext();

  void Update(double framerate); // TODO: framerate & time OR just time

  void ResetTime();

 private:
  // TODO: tinygltf::animation_samplers, so then we can delete all
  //  other data which are already loaded to vao/vbo/ebo
  void* animation_data_ = nullptr;

  int cur_animation_id_ = -1;
  int next_animation_id_ = -1;

  double time_ = 0;
  bool repeat_ = false;
};


// TODO: linear / step / spline (cubic?)

AnimTransform AnimTransformByTime(unsigned int global_offset,
                                  unsigned int global_size,
                                  unsigned int local_size, float time);
/// simple linear interpolation
glm::vec3 AnimPositionByTime(float time);
glm::quat AnimRotationByTime(float time);
glm::vec3 AnimScalingByTime(float time);
void ProcessAnimationKey(float time, const glm::mat4& parent_transform,
                         utility::Span<glm::mat4>& out_bone_transform,
                         const glm::mat4& global_inverse_transform_);



} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ANIMATION_H_
