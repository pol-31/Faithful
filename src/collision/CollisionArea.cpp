#include "CollisionArea.h"

#include <memory>
#include <cstring>

#include "Aabb.h"
#include "Shapes.h"

namespace faithful {

void CollisionArea::ComputeCollisionArea(std::vector<glm::vec3> vertices) {
  auto rectangle = ComputeBoundingRectangle(vertices);
  aabb_.min = rectangle.min;
  aabb_.max = rectangle.max;
  type_ = ComputeOptimalShape(vertices);
  switch (type_) {
    case Type::kRectangle:
    {
      data_ = std::make_unique<uint8_t[]>(sizeof(details::collision::Rectangle));
      memcpy(data_.get(), &rectangle, sizeof(details::collision::Rectangle));
    }
    break;
    case Type::kCircle:
    {
      auto circle = ComputeBoundingCircle(vertices);
      data_ = std::make_unique<uint8_t[]>(sizeof(details::collision::Circle));
      memcpy(data_.get(), &circle, sizeof(details::collision::Circle));
    }
    break;
    case Type::kDop8:
    {
      auto dop8 = ComputeBoundingK8Dop(vertices);
      data_ = std::make_unique<uint8_t[]>(sizeof(details::collision::Dop8));
      memcpy(data_.get(), &dop8, sizeof(details::collision::Dop8));
    }
    break;
    case Type::kSsLine:
    {
      auto ss_line = ComputeBoundingSsLine(vertices);
      data_ = std::make_unique<uint8_t[]>(sizeof(details::collision::SsLine));
      memcpy(data_.get(), &ss_line, sizeof(details::collision::SsLine));
    }
    break;
    case Type::kConvexHull:
    {
      auto convex_hull = ComputeBoundingConvexHull(vertices);
      data_ = std::make_unique<uint8_t[]>(sizeof(details::collision::ConvexHull));
      memcpy(data_.get(), &convex_hull, sizeof(details::collision::ConvexHull));
    }
    break;
  }
}

CollisionArea::Type CollisionArea::ComputeOptimalShape(
    std::vector<glm::vec3> vertices,
    const details::collision::Aabb& aabb) {
  return Type::kRectangle;
  // TODO:
  //  if rectangle ~ square -> circle
  //  if rectangle big -> k8dop
  //  if rectangle too big -> convex_hull
  //  if rectangle too narrow (x / y) -> ssLine
}

CollisionArea::Type CollisionArea::ComputeOptimalShape(
    std::vector<glm::vec3> vertices) {
  auto rectangle = ComputeBoundingRectangle(vertices);
  details::collision::Aabb aabb(rectangle.min, rectangle.max);
  ComputeOptimalShape(vertices, aabb);
}

details::collision::Rectangle CollisionArea::ComputeBoundingRectangle(
    std::vector<glm::vec3> vertices) {
  details::collision::Rectangle rectangle;
  rectangle.min = glm::vec2(std::numeric_limits<float>::max());
  rectangle.max = glm::vec2(std::numeric_limits<float>::lowest());
  for (const auto& vertex : vertices) {
    if (vertex.x < rectangle.min.x) {
      rectangle.min.x = vertex.x;
    }
    if (vertex.x > rectangle.max.x) {
      rectangle.max.x = vertex.x;
    }
    if (vertex.z < rectangle.min.y) {
      rectangle.min.y = vertex.z;
    }
    if (vertex.z > rectangle.max.y) {
      rectangle.max.y = vertex.z;
    }
  }
  return rectangle;
}

details::collision::Circle CollisionArea::ComputeBoundingCircle(
    std::vector<glm::vec3> vertices) {
  details::collision::Circle circle;
  EigenCircle(circle, vertices);
  for (auto& v : vertices) {
    CircleOfCircleAndPt(circle, glm::vec2{v.x, v.z});
  }
  return circle;
}

details::collision::Dop8 CollisionArea::ComputeBoundingK8Dop(
    std::vector<glm::vec3> vertices) {
  details::collision::Dop8 dop8;
  dop8.min[0] = dop8.min[1] = FLT_MAX;
  dop8.max[0] = dop8.max[1] = -FLT_MAX;
  float value;
  for (const auto& v : vertices) {
    // Axis 0 = (1,1)
    value = v.x + v.z;
    if (value < dop8.min[0]) {
      dop8.min[0] = value;
    } else if (value > dop8.max[0]) {
      dop8.max[0] = value;
    }
    // Axis 1 = (-1,1)
    value = -v.x + v.z;
    if (value < dop8.min[1]) {
      dop8.min[1] = value;
    } else if (value > dop8.max[1]) {
      dop8.max[1] = value;
    }
    // Axis 2 = (-1,-1)
    value = -v.x - v.z;
    if (value < dop8.min[2]) {
      dop8.min[2] = value;
    } else if (value > dop8.max[2]) {
      dop8.max[2] = value;
    }
    // Axis 3 = (1,-1)
    value = v.x - v.z;
    if (value < dop8.min[3]) {
      dop8.min[3] = value;
    } else if (value > dop8.max[3]) {
      dop8.max[3] = value;
    }
  }
  return dop8;
}

details::collision::SsLine CollisionArea::ComputeBoundingSsLine(
    std::vector<glm::vec3> vertices) {
  std::terminate(); // TODO: NOT IMPLEMENTED
}

details::collision::ConvexHull CollisionArea::ComputeBoundingConvexHull(
    std::vector<glm::vec3> vertices) {
  std::terminate(); // TODO: NOT IMPLEMENTED
}

float CollisionArea::Variance(std::vector<glm::vec3> vertices) {
  float u = 0.0f;
  for (const auto& v : vertices) {
    u += v.x;
    u += v.z;
  }
  u /= (vertices.size() * 2);
  float s2 = 0.0f;
  for (const auto& v : vertices) {
    s2 += (v.x - u) * (v.x - u);
    s2 += (v.z - u) * (v.z - u);
  }
  return s2 / (vertices.size() * 2);
}

void CollisionArea::CovarianceMatrix(
    glm::mat2 &cov, std::vector<glm::vec3> vertices) {
  float oon = 1.0f / static_cast<float>(vertices.size());
  glm::vec2 c = glm::vec2(0.0f, 0.0f);
  float e00, e11, e01;
  for (const auto& v : vertices) {
    c += glm::vec2{v.x, v.z};
  }
  c *= oon;
  e00 = e11 = e01 = 0.0f;
  for (const auto& v : vertices) {
    glm::vec2 p = glm::vec2{v.x, v.z} - c;
    e00 += p.x * p.x;
    e11 += p.y * p.y;
    e01 += p.x * p.y;
    c += c;
  }
  cov[0][0] = e00 * oon;
  cov[1][1] = e11 * oon;
  cov[0][1] = cov[1][0] = e01 * oon;
}

void CollisionArea::Jacobi(glm::mat2 &a, glm::mat2 &v) {
  int i, j, n, p, q;
  float prevoff, c, s;
  glm::mat2 J, b, t;
  for (i = 0; i < 3; i++) {
    v[i][0] = v[i][1] = v[i][2] = 0.0f;
    v[i][i] = 1.0f;
  }
  const int MAX_ITERATIONS = 50;
  for (n = 0; n < MAX_ITERATIONS; n++) {
    p = 0; q = 1;
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        if (i == j) {
          continue;
        }
        if (glm::abs(a[i][j]) > glm::abs(a[p][q])) {
          p = i;
          q = j;
        }
      }
    }
    SymSchur2(a, p, q, c, s);
    for (i = 0; i < 3; i++) {
      J[i][0] = J[i][1] = J[i][2] = 0.0f;
      J[i][i] = 1.0f;
    }
    J[p][p] = c; J[p][q] = s;
    J[q][p] = -s; J[q][q] = c;
    v = v * J;
    a = (glm::transpose(J) * a) * J;
    float off = 0.0f;
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        if (i == j) {
          continue;
        }
        off += a[i][j] * a[i][j];
      }
    }
    if (n > 2 && off >= prevoff) {
      return;
    }
    prevoff = off;
  }
}

void CollisionArea::SymSchur2(
    glm::mat2 &a, int p, int q, float &c, float &s) {
  if (glm::abs(a[p][q]) > 0.0001f) {
    float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
    float t;
    if (r >= 0.0f) {
      t = 1.0f / (r + glm::sqrt(1.0f + r*r));
    } else {
      t = -1.0f / (-r + glm::sqrt(1.0f + r*r));
    }
    c = 1.0f / glm::sqrt(1.0f + t*t);
    s = t * c;
  } else {
    c = 1.0f;
    s = 0.0f;
  }
}

void CollisionArea::ExtremePointsAlongDirection(
    glm::vec2 dir, std::vector<glm::vec3> vertices,
    int *imin, int *imax) {
  float minproj = FLT_MAX, maxproj = -FLT_MAX;
  for (int i = 0; i < vertices.size(); i++) {
    float proj = glm::dot({vertices[i].x, vertices[i].z}, dir);
    if (proj < minproj) {
      minproj = proj;
      *imin = i;
    }
    if (proj > maxproj) {
      maxproj = proj;
      *imax = i;
    }
  }
}

void CollisionArea::EigenCircle(
    details::collision::Circle &eigCircle,
    std::vector<glm::vec3> vertices) {
  glm::mat2 m, v;
  CovarianceMatrix(m, vertices);
  Jacobi(m, v);
  glm::vec2 e;
  int maxc = 0;
  float maxf, maxe = glm::abs(m[0][0]);
  if ((maxf = glm::abs(m[1][1])) > maxe) {
    maxc = 1;
    maxe = maxf;
  }
  if ((maxf = glm::abs(m[2][2])) > maxe) {
    maxc = 2;
    maxe = maxf;
  }
  e[0] = v[0][maxc];
  e[1] = v[1][maxc];
  e[2] = v[2][maxc];
  int imin, imax;
  ExtremePointsAlongDirection(e, vertices, &imin, &imax);
  glm::vec2 minpt = {vertices[imin].x, vertices[imin].z};
  glm::vec2 maxpt = {vertices[imax].x, vertices[imax].z};
  float dist = glm::sqrt(glm::dot(maxpt - minpt, maxpt - minpt));
  eigCircle.radius = dist * 0.5f;
  eigCircle.centre = (minpt + maxpt) * 0.5f;
}

void CollisionArea::CircleOfCircleAndPt(
    details::collision::Circle &s, glm::vec2 p) {
  glm::vec2 d = p - s.centre;
  float dist2 = glm::dot(d, d);
  if (dist2 > s.radius * s.radius) {
    float dist = glm::sqrt(dist2);
    float newRadius = (s.radius + dist) * 0.5f;
    float k = (newRadius - s.radius) / dist;
    s.radius = newRadius;
    s.centre += d * k;
  }
}

} // namespace faithful
