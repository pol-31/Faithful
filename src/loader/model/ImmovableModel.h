#ifndef FAITHFUL_SRC_LOADER_IMMOVABLEMODEL_H_
#define FAITHFUL_SRC_LOADER_IMMOVABLEMODEL_H_

#include "../Model.h"
#include "../../common/Object.h"

#include "Common.h"

namespace faithful {

class ImmovableModel :
    public faithful::Model,
    public Transformable, public Collidable,
    public details::assets::CommonModelDrawer {
 public:
  using Base = faithful::Model;
  using Base::Base;
  using Base::operator=;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_IMMOVABLEMODEL_H_
