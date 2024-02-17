#ifndef FAITHFUL_SRC_GUI_PUSHBUTTON_H_
#define FAITHFUL_SRC_GUI_PUSHBUTTON_H_

#include "../collision/Collision.h"
#include "Gui.h"
#include "../common/Object.h"

namespace faithful {

// Not animatable - button shading implemented with uniform variables
class PushButton : public IGui, public SoundEmittable {
 public:
  PushButton() {
    collision_ = Collision::CreateCollisionArea(
        Collision::Scene::kMainMenu, Collision::Type::kBoundingBox2D);
  }
  PushButton() {
    //
  }

  bool Pressed() {
    return pressed_;
  }
  void Press() {
    pressed_ = true;
  }
  void Release() {
    pressed_ = false;
  }

  // TODO: draw should set uniform variables related to "hovard" / "shading"

 public:
  // TODO: texture (all buttons has rectangular texture + A channel)
  //    so all buttons have the same rectangular shader
  bool pressed_ = false;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_GUI_PUSHBUTTON_H_
