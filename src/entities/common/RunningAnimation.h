#ifndef FAITHFUL_RUNNINGANIMATION_H
#define FAITHFUL_RUNNINGANIMATION_H

namespace faithful {

class SkinnedObject3DImpl;

class RunningAnimation {
 public:
  RunningAnimation() = default;
  RunningAnimation(SkinnedObject3DImpl* obj_info, int animation_id,
                   unsigned int ubo, bool repeat)
      : obj_info_(obj_info),
        animation_id_(animation_id),
        obj_ubo_(ubo),
        repeat_(repeat) {
  }

  void UpdateFrame(double framerate);
  bool Done() {
    return time_ == 1;
  }

  float get_speed() const {
    return speed_;
  }
  void set_speed(float speed) {
    speed_ = speed;
  }

 private:
  SkinnedObject3DImpl* obj_info_;
  unsigned int obj_ubo_;

  // TODO: Model:: assimp loading in two passes
  // TODO: create animation from animation (e.g. partial looping)
  // TODO: serialization (default shaders, objects, images, models, audios)
  // TODO: PBR (its time.......)

  int animation_id_;
  int next_animation_id_ = -1;
  //  TODO: time -> range [0; 1]
  /// time == -1 --> animation_ended
  double time_ = 0;
  float speed_ = 1.0f;
  bool repeat_;
};

}  // namespace faithful

#endif  // FAITHFUL_RUNNINGANIMATION_H
