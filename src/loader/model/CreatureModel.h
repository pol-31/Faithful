#ifndef FAITHFUL_SRC_LOADER_CREATUREMODEL_H_
#define FAITHFUL_SRC_LOADER_CREATUREMODEL_H_

#include "../Model.h"
#include "../../common/Object.h"

#include "Common.h"

namespace faithful {

class CreatureModel :
    public faithful::Model,
    public Transformable, public Animatable,
    public Collidable, public details::assets::CommonModelDrawer,
    public SoundEmittable, public Updatable {
 public:
  using Base = faithful::Model;
  using Base::Base;
  using Base::operator=;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_CREATUREMODEL_H_
