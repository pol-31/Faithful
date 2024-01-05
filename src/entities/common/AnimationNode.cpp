#include "AnimationNode.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
// TODO: #include <glm/gtc/quaternion-simd.hpp>

#include <algorithm>

#include <assimp/scene.h>

#include "../../../utils/Span.h"

#include <iostream>

namespace faithful {

AnimationNode::~AnimationNode() {
  delete bone_transform_;
  for (AnimationNode* c : children_) {
    c->~AnimationNode();
  }
}

AnimationNode::AnimationNode(const aiNodeAnim* node, glm::mat4 bone_transform,
                             glm::mat4 node_transform, int bone_id,
                             std::size_t children_num)
    : bone_id_(bone_id) {
  AssimpNodeAnimToSimurghAnimationNode(node);
  bone_transform_ = new glm::mat4(bone_transform);
  node_transform_ = new glm::mat4(node_transform);
  children_ = utility::Span<AnimationNode*>(children_num);
}

void AnimationNode::InsertNode(std::size_t child_num,
                               AnimationNode* child_node) {
  // assert(child_num < children_.get_size());
  //   - always true (managed by Dev)
  children_[child_num] = child_node;
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

// TODO: heap sort better
void AnimationNode::AssimpNodeAnimToSimurghAnimationNode(
    const aiNodeAnim* node) {
  if (node == nullptr)
    return;
  keys_ = utility::Span<float>(
      4 * (node->mNumScalingKeys + node->mNumPositionKeys) +
      5 * node->mNumRotationKeys);
  std::size_t cur_pos = 0;
  for (int i = 0; i < node->mNumScalingKeys; ++i) {
    keys_[cur_pos++] = node->mScalingKeys[i].mValue.x;
    keys_[cur_pos++] = node->mScalingKeys[i].mValue.y;
    keys_[cur_pos++] = node->mScalingKeys[i].mValue.z;
    keys_[cur_pos++] = node->mScalingKeys[i].mTime;
  }
  offset_rotation_and_position_ =
      cur_pos << (sizeof(unsigned int) * 8 / 2);  // upper 2 bytes
  for (int i = 0; i < node->mNumRotationKeys; ++i) {
    keys_[cur_pos++] = node->mRotationKeys[i].mValue.w;
    keys_[cur_pos++] = node->mRotationKeys[i].mValue.x;
    keys_[cur_pos++] = node->mRotationKeys[i].mValue.y;
    keys_[cur_pos++] = node->mRotationKeys[i].mValue.z;
    keys_[cur_pos++] = node->mRotationKeys[i].mTime;
  }
  offset_rotation_and_position_ += cur_pos;  // lower 2 bytes
  for (int i = 0; i < node->mNumPositionKeys; ++i) {
    keys_[cur_pos++] = node->mPositionKeys[i].mValue.x;
    keys_[cur_pos++] = node->mPositionKeys[i].mValue.y;
    keys_[cur_pos++] = node->mPositionKeys[i].mValue.z;
    keys_[cur_pos++] = node->mPositionKeys[i].mTime;
  }
}

}  // namespace faithful
