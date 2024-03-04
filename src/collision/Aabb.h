#ifndef FAITHFUL_SRC_COLLISION_AABB_H_
#define FAITHFUL_SRC_COLLISION_AABB_H_

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

namespace faithful {
namespace details {
namespace collision {

struct Aabb {
 public:
  Aabb() {
    float minNum = std::numeric_limits<float>::lowest();
    float maxNum = std::numeric_limits<float>::max();
    min = {maxNum, maxNum};
    max = {minNum, minNum};
  }

  Aabb(const Aabb&) = default;
  Aabb(Aabb&&) = default;
  Aabb& operator=(const Aabb&) = default;
  Aabb& operator=(Aabb&&) = default;

  Aabb(glm::vec2 p)
      : min(p),
        max(p) {
  }
  Aabb(glm::vec2 p1, glm::vec2 p2)
      : min(glm::min(p1, p2)),
        max(glm::max(p1, p2)) {
  }

  glm::vec2 Diagonal() const {
    return max - min;
  }
  bool Empty() const {
    return min.x >= max.x || min.y >= max.y;
  }
  bool Degenerate() const {
    return min.x > max.x || min.y > max.y;
  }

  int MaxDimension() const {
    glm::vec2 diagonal = Diagonal();
    return (diagonal.x > diagonal.y) ? 0 : 1;
  }

  float SurfaceArea() const {
    glm::vec2 diagonal = Diagonal();
    return diagonal.x * diagonal.y;
  }

  glm::vec2 operator[](int i) const {
    return (i == 0) ? min : max;
  }

  glm::vec2& operator[](int i) {
    return (i == 0) ? min : max;
  }

  bool operator==(const Aabb& other) const {
    return other.min == min && other.max == max;
  }

  bool operator!=(const Aabb& other) const {
    return other.min != min || other.max != max;
  }

  glm::vec2 Corner(int corner) const {
    return {(*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y};
  }

  glm::vec2 Lerp(glm::vec2 t) const {
    return glm::vec2(glm::lerp(t.x, min.x, max.x),
                     glm::lerp(t.y, min.y, max.y));
  }

  glm::vec2 Offset(glm::vec2 p) const {
    glm::vec2 o = p - min;
    if (max.x > min.x) {
      o.x /= max.x - min.x;
    }
    if (max.y > min.y) {
      o.y /= max.y - min.y;
    }
    return o;
  }

  void BoundingSphere(glm::vec2* c, float* rad) const;

  // TODO: std::string ToString() const;

  /// intentionally public
  glm::vec2 min;
  glm::vec2 max;
};

inline Aabb Union(const Aabb& b1, const Aabb& b2) {
  return {glm::min(b1.min, b2.min), glm::max(b1.max, b2.max)};
}

inline Aabb Union(const Aabb& b, glm::vec2 p) {
  return {glm::min(b.min, p), glm::max(b.max, p)};
}

inline Aabb Intersect(const Aabb& b1, const Aabb& b2) {
  return {glm::max(b1.min, b2.min), glm::min(b1.max, b2.max)};
}

inline bool Overlaps(const Aabb& ba, const Aabb& bb) {
  bool x = (ba.max.x >= bb.min.x) && (ba.min.x <= bb.max.x);
  bool y = (ba.max.y >= bb.min.y) && (ba.min.y <= bb.max.y);
  return (x && y);
}

inline bool Inside(glm::vec2 pt, const Aabb& b) {
  return (pt.x >= b.min.x && pt.x <= b.max.x && pt.y >= b.min.y &&
          pt.y <= b.max.y);
}

inline bool Inside(const Aabb& ba, const Aabb& bb) {
  return (ba.min.x >= bb.min.x && ba.max.x <= bb.max.x &&
          ba.min.y >= bb.min.y && ba.max.y <= bb.max.y);
}

inline bool InsideExclusive(glm::vec2 pt, const Aabb& b) {
  return (pt.x >= b.min.x && pt.x < b.max.x && pt.y >= b.min.y &&
          pt.y < b.max.y);
}

inline Aabb Expand(const Aabb& b, float delta) {
  return {b.min - glm::vec2(delta, delta), b.max + glm::vec2(delta, delta)};
}

void Aabb::BoundingSphere(glm::vec2* c, float* rad) const {
  *c = (min + max) / 2.0f;
  *rad = Inside(*c, *this) ? glm::distance(*c, max) : 0;
}

} // namespace collision
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_AABB_H_
