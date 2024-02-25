#include "Bvh.h"

#include "../loader/AssetInstanceInfo.h"

namespace faithful {
namespace details {

void Bvh::Init() {
  // TODO: discard all "inactive" models

  std::vector<BVHPrimitive> bvhPrimitives(primitives.size());
  for (size_t i = 0; i < primitives.size(); ++i)
    bvhPrimitives[i] = BVHPrimitive(i, primitives[i].Bounds());
}


Bvh::Bvh(const assets::ModelPool& model_pool,
         const PlayerCharacter& player_character,
         const environment::PhenomenonAreaPool& phenomenon_area_pool) {

  Init();
  Update();
}

void Bvh::Update() {
  int secondary_sample_id = 1 - cur_sample_id_;
  samples_[secondary_sample_id].root_ = BuildRecursive();
  cur_sample_id_ = secondary_sample_id;
}

// TODO: node_id + subspan mid
Bvh::BVHBuildNode* Bvh::BuildRecursive(int node_id) {
  BVHBuildNode *node = samples_[cur_sample_id_].nodes_[node_id];
  Aabb bounds;
  for (const auto &prim : bvhPrimitives) {
    bounds = Union(bounds, prim.bounds);
  }
  if (bounds.SurfaceArea() == 0 || bvhPrimitives.size() == 1) {
    node->InitLeaf(bvhPrimitives.size(), bounds);
    return node;
  }

  Aabb centroidBounds;
  for (const auto &prim : bvhPrimitives)
    centroidBounds = Union(centroidBounds, prim.Centroid());
  int dim = centroidBounds.MaxDimension();

  if (centroidBounds.max_[dim] == centroidBounds.min_[dim]) {
    node->InitLeaf(bvhPrimitives.size(), bounds);
    return node;
  } else {
    int mid = bvhPrimitives.size() / 2;
    if (bvhPrimitives.size() <= 2) {
      mid = bvhPrimitives.size() / 2;
      std::nth_element(bvhPrimitives.begin(), bvhPrimitives.begin() + mid,
                       bvhPrimitives.end(),
                       [dim](const BVHPrimitive &a, const BVHPrimitive &b) {
                         return a.Centroid()[dim] < b.Centroid()[dim];
                       });
    } else {
      constexpr int nBuckets = 12;
      BVHSplitBucket buckets[nBuckets];
      for (const auto &prim : bvhPrimitives) {
        int b = nBuckets * centroidBounds.Offset(prim.Centroid())[dim];
        if (b == nBuckets)
          b = nBuckets - 1;
        buckets[b].count++;
        buckets[b].bounds = Union(buckets[b].bounds, prim.bounds);
      }

      constexpr int nSplits = nBuckets - 1;
      float costs[nSplits] = {};
      int countBelow = 0;
      Aabb boundBelow;
      for (int i = 0; i < nSplits; ++i) {
        boundBelow = Union(boundBelow, buckets[i].bounds);
        countBelow += buckets[i].count;
        costs[i] += countBelow * boundBelow.SurfaceArea();
      }

      int countAbove = 0;
      Aabb boundAbove;
      for (int i = nSplits; i >= 1; --i) {
        boundAbove = Union(boundAbove, buckets[i].bounds);
        countAbove += buckets[i].count;
        costs[i - 1] += countAbove * boundAbove.SurfaceArea();
      }

      int minCostSplitBucket = -1;
      float minCost = Infinity;
      for (int i = 0; i < nSplits; ++i) {
        if (costs[i] < minCost) {
          minCost = costs[i];
          minCostSplitBucket = i;
        }
      }
      float leafCost = bvhPrimitives.size();
      minCost = 1.f / 2.f + minCost / bounds.SurfaceArea();

      if (bvhPrimitives.size() > maxPrimsInNode || minCost < leafCost) {
        auto midIter = std::partition(
            bvhPrimitives.begin(), bvhPrimitives.end(),
            [=](const BVHPrimitive &bp) {
              int b =
                  nBuckets * centroidBounds.Offset(bp.Centroid())[dim];
              if (b == nBuckets)
                b = nBuckets - 1;
              return b <= minCostSplitBucket;
            });
        mid = midIter - bvhPrimitives.begin();
      } else {
        node->InitLeaf(bvhPrimitives.size(), bounds);
        return node;
      }
    }

    BVHBuildNode *children[2];
    children[0] =
        buildRecursive(bvhPrimitives.subspan(0, mid));
    children[1] =
        buildRecursive(bvhPrimitives.subspan(mid));

    node->InitInterior(dim, children[0], children[1]);
  }
  return node;
}

void Bvh::ProcessSah() {
  //
}

/*
pstd::optional<ShapeIntersection> Bvh::Intersect(
    const Ray &ray, float tMax) const {
  if (!nodes)
    return {};
  pstd::optional<ShapeIntersection> si;
  glm::vec3 invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
  int dirIsNeg[3] = {int(invDir.x < 0), int(invDir.y < 0), int(invDir.z < 0)};
  // Follow ray through BVH nodes to find primitive intersections
  int toVisitOffset = 0, currentNodeIndex = 0;
  int nodesToVisit[64];
  int nodesVisited = 0;
  while (true) {
    ++nodesVisited;
    const LinearBVHNode *node = &nodes[currentNodeIndex];
    // Check ray against BVH node
    if (node->bounds.IntersectP(ray.o, ray.d, tMax, invDir, dirIsNeg)) {
      if (node->nPrimitives > 0) {
        // Intersect ray with primitives in leaf BVH node
        for (int i = 0; i < node->nPrimitives; ++i) {
          // Check for intersection with primitive in BVH node
          pstd::optional<ShapeIntersection> primSi =
              primitives[node->primitivesOffset + i].Intersect(ray, tMax);
          if (primSi) {
            si = primSi;
            tMax = si->tHit;
          }
        }
        if (toVisitOffset == 0)
          break;
        currentNodeIndex = nodesToVisit[--toVisitOffset];

      } else {
        // Put far BVH node on _nodesToVisit_ stack, advance to near node
        if (dirIsNeg[node->axis]) {
          nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
          currentNodeIndex = node->secondChildOffset;
        } else {
          nodesToVisit[toVisitOffset++] = node->secondChildOffset;
          currentNodeIndex = currentNodeIndex + 1;
        }
      }
    } else {
      if (toVisitOffset == 0)
        break;
      currentNodeIndex = nodesToVisit[--toVisitOffset];
    }
  }

  bvhNodesVisited += nodesVisited;
  return si;
}
*/


} // namespace details
} // namespace faithful
