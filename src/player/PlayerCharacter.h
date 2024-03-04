#ifndef FAITHFUL_SRC_ENTITIES_PLAYERCHARACTER_H_
#define FAITHFUL_SRC_ENTITIES_PLAYERCHARACTER_H_

#include "../loader/model/AnimatableModel.h"

namespace faithful {

class PlayerCharacter : public AnimatableModel {
 public:
  using Base = AnimatableModel;
  // TODO: accessories, hp/damage, loot
 private:
  using Base::transform_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENTITIES_PLAYERCHARACTER_H_
