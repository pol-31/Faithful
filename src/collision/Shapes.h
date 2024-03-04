#ifndef FAITHFUL_SRC_COLLISION_SHAPES_H_
#define FAITHFUL_SRC_COLLISION_SHAPES_H_

#include <glm/glm.hpp>

namespace faithful {
namespace details {
namespace collision {

struct Rectangle {
  glm::vec2 min;
  glm::vec2 max;
};

struct Circle {
  glm::vec2 centre;
  float radius;
};

struct Dop8 {
  float min[4];
  float max[4];
};

struct SsLine {
  glm::vec2 start;
  glm::vec2 end;
  float radius;
};

struct ConvexHull {
  std::unique_ptr<float[]> vertices;
  std::size_t size;
};

struct Slab {
  float normal[3];
  float near_distance;
  float far_distance;
};

struct HalfPlane;
struct Ray;
struct Disk;
struct CircleSector; // (?) - like triangle and like cone but in 2d and like.... idk

} // namespace collision
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_SHAPES_H_
