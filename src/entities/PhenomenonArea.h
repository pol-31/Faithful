#ifndef FAITHFUL_SRC_ENVIRONMENT_PHENOMENONFIELD_H_
#define FAITHFUL_SRC_ENVIRONMENT_PHENOMENONFIELD_H_

#include "../loader/IAssetPool.h"
#include "../loader/AssetBase.h"

#include "../collision/Aabb.h"

#include "glm/glm.hpp"

namespace faithful {

class PhenomenonAreaPool
    : public details::assets::IAssetPool<
          config::max_instances::kPhenomenonArea> {
 public:
  using TransformationDataType =
      std::array<glm::mat3, config::max_instances::kPhenomenonArea>;
  using CollisionDataType =
      std::array<Aabb, config::max_instances::kPhenomenonArea>;

  const CollisionDataType& GetCollisionData() const {
    return collision_data_;
  }
  const TransformationDataType& GetTransformationData() const {
    return transformation_data_;
  }

 private:
  TransformationDataType transformation_data_;
  CollisionDataType collision_data_;
};


// TODO: inherit from Transformable?
class PhenomenonArea : public details::assets::AssetBase {
  //
};

} // namespace faithful


#endif  // FAITHFUL_SRC_ENVIRONMENT_PHENOMENONFIELD_H_
