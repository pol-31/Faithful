#include "Animation.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/quaternion.hpp"
// TODO: #include <glm/gtc/quaternion-simd.hpp>

#include <algorithm>

#include "../../utils/Span.h"

#include <iostream>

#include "glad/include/glad/glad.h"
#include "glm/glm/glm.hpp"

namespace faithful {

void Animation::Run(int animation_id) {
  UnsetNext();
  ResetTime();
  SetCurrent(animation_id);
}

void Animation::Stop() {
  UnsetNext();
  UnsetCurrent();
  ResetTime();
}

void Animation::SetCurrent(int animation_id) {
  ResetTime();
  cur_animation_id_ = animation_id;
}
void Animation::UnsetCurrent() {
  ResetTime();
  cur_animation_id_ = next_animation_id_;
  UnsetNext();
}

void Animation::SetNext(int animation_id) {
  next_animation_id_ = animation_id;
}
void Animation::UnsetNext() {
  next_animation_id_ = -1;
}

void Animation::ResetTime() {
  time_ = 0;
//  time_ = glfwCurrentTime(); // TODO: -___________---_--___--__-__--___-
}


void RunningAnimation::UpdateFrame(double framerate) {
  /// Case (time == 1) is checked in hierarchy higher
  /// (SkinnedObjectImpl::Update())
  time_ += framerate * speed_ / 10;

  // time_ * 200 /* TODO: * TicksPerSeconds*/

  if (time_ >= 1) {
    if (repeat_) {
      time_ = 0;
    } else {
      time_ = 1;
      return;
    }
  }

  obj_info_->animation_nodes_[animation_id_]->ProcessAnimationKey(
      time_ * 200, glm::mat4(1.0f), obj_info_->cur_pose_,
      obj_info_->global_inverse_transform_);
  glBindBuffer(GL_UNIFORM_BUFFER, obj_ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0,
                  obj_info_->bone_num_ * sizeof(glm::mat4),
                  glm::value_ptr(obj_info_->cur_pose_[0]));
  // obj_info_->DefaultPose(obj_ubo_);
}

void AnimationNode::ProcessAnimationKey(
    float time, const glm::mat4& parent_transform,
    utility::Span<glm::mat4>& out_bone_transform,
    const glm::mat4& global_inverse_transform_) {
  glm::mat4 node_transform(*node_transform_);
  if (!keys_.Empty()) {
    glm::mat4 animation_s =
        glm::scale(glm::mat4(1.0f), AnimScalingByTime(time));
    glm::mat4 animation_r = glm::mat4_cast(AnimRotationByTime(time));
    glm::mat4 animation_t =
        glm::translate(glm::mat4(1.0f), AnimPositionByTime(time));
    node_transform = animation_t * animation_r * animation_s;
  }
  glm::mat4 cur_transform = parent_transform * node_transform;
  //  std::cout << bone_id_ << std::endl;
  // TODO: mb its better just not to use global_inverse_transform_ (?)
  out_bone_transform[bone_id_] =
      /*global_inverse_transform_ **/ cur_transform * (*bone_transform_);
  for (int i = 0; i < children_.get_size(); ++i) {
    children_[i]->ProcessAnimationKey(time, cur_transform, out_bone_transform,
                                      global_inverse_transform_);
  }
}

AnimationNode::AnimTransform AnimationNode::AnimTransformByTime(
    unsigned int global_offset, unsigned int global_size,
    unsigned int local_size, float time) {
  if (time == 0.0f)
    return {global_offset, global_offset, 0};
  // TODO: separate check for (global_size == 0); separate check for (time == 0)
  if (global_size <= local_size)
    return {global_offset, global_offset, 0};
  /// all our values sorted by time, so std::lower_bound
  /// (i.e. Binary Search Algorithm) is the best choice
  SkipIterator<float> begin(&*(keys_.begin() + global_offset + local_size - 1),
                            local_size);
  /// last is not included (by std::lower_bound definition)
  // TODO: "- local_size"
  SkipIterator<float> end(
      &*(keys_.begin() + global_offset + global_size - local_size - 1),
      local_size);

  int test_size = 0;
  for (auto it = begin; it != end; ++it) {
    ++test_size;
  }
  auto resultIterator = std::lower_bound(begin, end, time);

  if ((resultIterator == begin) ||
      ((resultIterator == end) && (*resultIterator < time))) {
    unsigned int offset =
        std::distance(keys_.get_data(), &*end) - local_size + 1;
    // TODO: we don't need result continiously (every frame) but sometimes only
    // once
    return {offset, offset, 0};
  }
  unsigned int offset_last =
      std::distance(keys_.get_data(), &*resultIterator) - local_size + 1;
  unsigned int offset_first = offset_last - local_size;
  float time_first = keys_[offset_first + local_size - 1];
  float time_last = keys_[offset_last + local_size - 1];

  return {offset_first, offset_last,
          (time - time_first) / (time_last - time_first)};
}

glm::vec3 AnimationNode::AnimPositionByTime(float time) {
  unsigned int size = keys_.get_size() - get_position_offset();
  /// position data: x, y, z, time (total 4)
  AnimTransform frame_info =
      AnimTransformByTime(get_position_offset(), size, 4, time);
  if (frame_info.factor == 0)
    return {keys_[frame_info.idx_first], keys_[frame_info.idx_first + 1],
            keys_[frame_info.idx_first + 2]};

  glm::vec3 start = {keys_[frame_info.idx_first],
                     keys_[frame_info.idx_first + 1],
                     keys_[frame_info.idx_first + 2]};
  glm::vec3 end = {keys_[frame_info.idx_last], keys_[frame_info.idx_last + 1],
                   keys_[frame_info.idx_last + 2]};
  return glm::mix(start, end, frame_info.factor);
}

glm::quat AnimationNode::AnimRotationByTime(float time) {
  unsigned int size = get_position_offset() - get_rotation_offset();
  /// position data: w, x, y, z, time (total 5)
  AnimTransform frame_info =
      AnimTransformByTime(get_rotation_offset(), size, 5, time);
  if (frame_info.factor == 0)
    return {keys_[frame_info.idx_first], keys_[frame_info.idx_first + 1],
            keys_[frame_info.idx_first + 2], keys_[frame_info.idx_first + 3]};

  glm::quat start = {
      keys_[frame_info.idx_first], keys_[frame_info.idx_first + 1],
      keys_[frame_info.idx_first + 2], keys_[frame_info.idx_first + 3]};
  glm::quat end = {keys_[frame_info.idx_last], keys_[frame_info.idx_last + 1],
                   keys_[frame_info.idx_last + 2],
                   keys_[frame_info.idx_last + 3]};
  return glm::mix(start, end, frame_info.factor);
}

glm::vec3 AnimationNode::AnimScalingByTime(float time) {
  unsigned int size = get_rotation_offset();  // size in float (not frame data)
  /// position data: x, y, z, time (total 4)
  AnimTransform frame_info = AnimTransformByTime(0, size, 4, time);
  if (frame_info.factor == 0)
    return {keys_[frame_info.idx_first], keys_[frame_info.idx_first + 1],
            keys_[frame_info.idx_first + 2]};

  glm::vec3 start = {keys_[frame_info.idx_first],
                     keys_[frame_info.idx_first + 1],
                     keys_[frame_info.idx_first + 2]};
  glm::vec3 end = {keys_[frame_info.idx_last], keys_[frame_info.idx_last + 1],
                   keys_[frame_info.idx_last + 2]};

  return glm::mix(start, end, frame_info.factor);
}


} // namespace faithful
