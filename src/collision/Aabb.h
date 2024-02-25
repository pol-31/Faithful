#ifndef FAITHFUL_SRC_COLLISION_AABB_H_
#define FAITHFUL_SRC_COLLISION_AABB_H_

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

namespace faithful {
namespace details {

class Aabb {
 public:
  Aabb() {
    float minNum = std::numeric_limits<float>::lowest();
    float maxNum = std::numeric_limits<float>::max();
    min_ = {maxNum, maxNum};
    max_ = {minNum, minNum};
  }

  Aabb(const Aabb&) = default;
  Aabb(Aabb&&) = default;
  Aabb& operator=(const Aabb&) = default;
  Aabb& operator=(Aabb&&) = default;

  Aabb(glm::vec2 p) : min_(p), max_(p) {}
  Aabb(glm::vec2 p1, glm::vec2 p2)
      : min_(glm::min(p1, p2)), max_(glm::max(p1, p2)) {}


  glm::vec2 Diagonal() const {
    return max_ - min_;
  }
  bool Empty() const {
    return min_.x >= max_.x || min_.y >= max_.y;
  }
  bool Degenerate() const {
    return min_.x > max_.x || min_.y > max_.y;
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
    return (i == 0) ? min_ : max_;
  }

  glm::vec2& operator[](int i) {
    return (i == 0) ? min_ : max_;
  }

  bool operator==(const Aabb& other) const {
    return other.min_ == min_ && other.max_ == max_;
  }

  bool operator!=(const Aabb& other) const {
    return other.min_ != min_ || other.max_ != max_;
  }

  glm::vec2 Corner(int corner) const {
    return {(*this)[(corner & 1)].x,
            (*this)[(corner & 2) ? 1 : 0].y};
  }

  glm::vec2 Lerp(glm::vec2 t) const {
    return glm::vec2(glm::lerp(t.x, min_.x, max_.x),
                     glm::lerp(t.y, min_.y, max_.y));
  }

  glm::vec2 Offset(glm::vec2 p) const {
    glm::vec2 o = p - min_;
    if (max_.x > min_.x) {
      o.x /= max_.x - min_.x;
    }
    if (max_.y > min_.y) {
      o.y /= max_.y - min_.y;
    }
    return o;
  }

  void BoundingSphere(glm::vec2 *c, float *rad) const;

  //TODO: std::string ToString() const;

  /// intentionally public
  glm::vec2 min_;
  glm::vec2 max_;
};

inline Aabb Union(const Aabb &b1, const Aabb &b2) {
  return {glm::min(b1.min_, b2.min_),
          glm::max(b1.max_, b2.max_)};
}

inline Aabb Union(const Aabb &b, glm::vec2 p) {
  return {glm::min(b.min_, p),
          glm::max(b.max_, p)};
}

inline Aabb Intersect(const Aabb &b1, const Aabb &b2) {
  return {glm::max(b1.min_, b2.min_),
          glm::min(b1.max_, b2.max_)};
}

inline bool Overlaps(const Aabb &ba, const Aabb &bb) {
  bool x = (ba.max_.x >= bb.min_.x) && (ba.min_.x <= bb.max_.x);
  bool y = (ba.max_.y >= bb.min_.y) && (ba.min_.y <= bb.max_.y);
  return (x && y);
}

inline bool Inside(glm::vec2 pt, const Aabb &b) {
  return (pt.x >= b.min_.x && pt.x <= b.max_.x &&
          pt.y >= b.min_.y && pt.y <= b.max_.y);
}

inline bool Inside(const Aabb &ba, const Aabb &bb) {
  return (ba.min_.x >= bb.min_.x && ba.max_.x <= bb.max_.x &&
          ba.min_.y >= bb.min_.y && ba.max_.y <= bb.max_.y);
}

inline bool InsideExclusive(glm::vec2 pt, const Aabb &b) {
  return (pt.x >= b.min_.x && pt.x < b.max_.x &&
          pt.y >= b.min_.y && pt.y < b.max_.y);
}

inline Aabb Expand(const Aabb &b, float delta) {
  return {b.min_ - glm::vec2(delta, delta),
          b.max_ + glm::vec2(delta, delta)};
}

void Aabb::BoundingSphere(glm::vec2 *c, float *rad) const {
  *c = (min_ + max_) / 2.0f;
  *rad = Inside(*c, *this) ? glm::distance(*c, max_) : 0;
}

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_AABB_H_
