#include "RunningAnimation.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "../Object.h"

namespace faithful {

void RunningAnimation::UpdateFrame(double framerate) {
  /// Case (time == 1) is checked in hierarchy higher (SkinnedObjectImpl::Update())
  time_ += framerate * speed_ / 10;

  //time_ * 200 /* TODO: * TicksPerSeconds*/

  if (time_ >= 1) {
    if (repeat_) {
      time_ = 0;
    } else {
      time_ = 1;
      return;
    }
  }

  obj_info_->animation_nodes_[animation_id_]->ProcessAnimationKey(
    time_ * 200,
    glm::mat4(1.0f),
    obj_info_->cur_pose_,
    obj_info_->global_inverse_transform_);
  glBindBuffer(GL_UNIFORM_BUFFER, obj_ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER,
                  0,
                  obj_info_->bone_num_ * sizeof(glm::mat4),
                  glm::value_ptr(obj_info_->cur_pose_[0]));
  //obj_info_->DefaultPose(obj_ubo_);
}

} // namespace faithful

