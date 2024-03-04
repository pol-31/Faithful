#ifndef FAITHFUL_SRC_ENTITIES_OBSTACLE_H_
#define FAITHFUL_SRC_ENTITIES_OBSTACLE_H_

#include "../loader/model/NonAnimatableModel.h"

namespace faithful {

class Obstacle : public NonAnimatableModel {
 public:
  using Base = NonAnimatableModel;
  using Base::Base;
  using Base::operator=;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENTITIES_OBSTACLE_H_
