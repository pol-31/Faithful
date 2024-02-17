#ifndef FAITHFUL_SRC_COLLISION_COLLISION_H_
#define FAITHFUL_SRC_COLLISION_COLLISION_H_

namespace faithful {

// TODO: collision detection implementation (trees/bvh4x4 for static/dynamic etc...)

// We DON'T really need 3D BVH and rather just 2D BVH tree,
// BUT we still need 3D collision check for jumps, falling (item/creature),
//   and different types of attach (sometimes you can evade from attack by sitting)

// So then we should separate "consistent" collision detection from "immediate"

// Consistent collision detection: should be updated dynamically in game
//      usage: exact game (we take A-buffer collision detection from UP?)
//             HUD collision detection. - BVH
//      so for consistent we have Static (hud) and Dynamic (game)

// Immediate collision detection: should check BVH-tree with event
//   even in this context means some temporary collision object which area
//   should be compared to BVH at some "linearization point"
//      usage: exact game

// for HUD because it static we can utilize quad-trees
// for game because it dynamic we can utilize BVH
//     and also rendering to A-buffer, so get TODO: Do we really need it?

class Collision {
 public:
  enum class Type {
    kBoundingBox2D,
    kBoundingBox3D,
    kCircle,
    kEllipse, // do we need it?
    kSphere,
    kCapsule
  };

  static Collision* CreateCollisionArea(Scene where,
                                        Type type);
};

} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_COLLISION_H_
