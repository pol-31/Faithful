#ifndef FAITHFUL_SRC_COLLISION_COLLISIONTARGET_H_
#define FAITHFUL_SRC_COLLISION_COLLISIONTARGET_H_

#include <glm/glm.hpp>

#include "Shapes.h"

namespace faithful {

enum class DamageType {
  kDefault, // the weakest
  kGround,  // can block only from sitting
  kMiddle,  // can block
  kHigh     // can block in any way + can just sit, but this is the most painful attack?
};

/// Possible shapes for attack: rectangle, circle, ray, cone, Disk

void Attack(DamageType, Shape, glm::vec2 position, float duration) {}

// SO we put each such attacks to CollisionManager::task_queue_ which
// then apply it for each entity that should be affected


} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_COLLISIONTARGET_H_
