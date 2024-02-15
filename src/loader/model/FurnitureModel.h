#ifndef FAITHFUL_SRC_LOADER_FURNITUREMODEL_H_
#define FAITHFUL_SRC_LOADER_FURNITUREMODEL_H_

#include "../Model.h"
#include "../../common/Object.h"

#include "Common.h"

namespace faithful {

class FurnitureModel :
    public faithful::Model,
    public Transformable3D, public Animatable,
    public Collidable, public details::assets::CommonModelDrawer {
 public:
  using Base = faithful::Model;
  using Base::Base;
  using Base::operator=;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_FURNITUREMODEL_H_
