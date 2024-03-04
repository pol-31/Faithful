#ifndef FAITHFUL_SRC_ENTITIES_BOSS_H_
#define FAITHFUL_SRC_ENTITIES_BOSS_H_

#include "../loader/model/AnimatableModel.h"

namespace faithful {

class Boss : public AnimatableModel {
 public:
  using Base = AnimatableModel;
  using Base::Base;
  using Base::operator=;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENTITIES_BOSS_H_
