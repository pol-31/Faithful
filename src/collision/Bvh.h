#ifndef FAITHFUL_SRC_COLLISION_BVH_H_
#define FAITHFUL_SRC_COLLISION_BVH_H_

#include <optional>
#include <memory_resource>
#include <atomic>
#include <vector>

#include "Aabb.h"
#include "../loader/ModelPool.h"
#include "../entities/PhenomenonArea.h"
#include "../player/PlayerCharacter.h"

namespace faithful {
namespace details {

namespace assets {

// TODO: need unified interface for this
//  (collision + bounding volume + transform)
struct ModelInstanceInfo;

} // namespace assets

class Bvh {
 public:
  using TransformationDataType =
      const std::array<glm::mat3, config::max_instances::kTotalInstances>&;

  Bvh() = delete;
  Bvh(const assets::ModelPool& model_pool,
      const PlayerCharacter& player_character,
      const environment::PhenomenonAreaPool& phenomenon_area_pool);


  struct BVHBuildNode {
    void InitLeaf(int n, const Aabb &b) {
      nPrimitives = n;
      bounds = b;
      children[0] = children[1] = nullptr;
    }

    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
      children[0] = c0;
      children[1] = c1;
      bounds = Union(c0->bounds, c1->bounds);
      splitAxis = axis;
      nPrimitives = 0;
    }

    Aabb bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
  };

  struct BVHSplitBucket {
    int count = 0;
    Aabb bounds;
  };

  void Update();


//  pstd::optional<ShapeIntersection> Bvh::Intersect(const Ray &ray, float tMax) const;

 private:
  /// needed if we start creating new tree, but some checks are still
  /// executed on previous tree (concurrency)
  struct Sample {
    std::vector<BVHBuildNode*> nodes_;
    BVHBuildNode* root_;
  };

  void ProcessSah();

  std::array<assets::ModelInstanceInfo*,
             config::max_instances::kTotalInstances> bvhPrimitives;

  void Init();

  BVHBuildNode* BuildRecursive(int node_id = 0);

  Sample samples_[2];
  int cur_sample_id_ = 0;

  int maxPrimsInNode = 2; // todo not sure
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_BVH_H_
