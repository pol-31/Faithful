#ifndef FAITHFUL_SRC_COLLISION_COLLISIONAREA_H_
#define FAITHFUL_SRC_COLLISION_COLLISIONAREA_H_

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Aabb.h"
#include "Shapes.h"

namespace faithful {

/// class for storing collision-related information
/// also includes functions to compute bounding shapes for
/// vertex array
class CollisionArea {
 public:
  enum class Type {
    kRectangle,
    kCircle,
    kDop8,
    kSsLine,
    kConvexHull
  };
  CollisionArea() = default;

  /// not copyable because of std::unique_ptr data
  CollisionArea(const CollisionArea&) = delete;
  CollisionArea& operator=(const CollisionArea&) = delete;

  CollisionArea(CollisionArea&&) = default;
  CollisionArea& operator=(CollisionArea&&) = default;

  Type GetType() const {
    return type_;
  }
  const std::unique_ptr<uint8_t[]>& GetData() const {
    return data_;
  }
  const details::collision::Aabb& GetAabb() const {
    return aabb_;
  }

  void ComputeCollisionArea(std::vector<glm::vec3> vertices);

  /// based on Aabb (bounding rectangle), so if caller don't provide it,
  /// we compute it on our own:
  /// - if rectangle ~ square -> circle
  /// - if rectangle small / normal size -> rectangle
  /// - if rectangle big -> k8dop
  /// - if rectangle big and narrow (x / y) -> ssLine
  /// - if rectangle too big -> convex_hull
  static Type ComputeOptimalShape(std::vector<glm::vec3> vertices,
                                  const details::collision::Aabb& aabb);
  static Type ComputeOptimalShape(std::vector<glm::vec3> vertices);

  /// Computes the bounding rectangle of a set of vertices.
  /// Iterates through all points to find the minimum and maximum coordinates.
  /// \param vertex_num The number of vertices in the input array.
  /// \param vertex_data Pointer to the array of vertex coordinates.
  /// \return A Rectangle structure representing the bounding rectangle.
  static details::collision::Rectangle ComputeBoundingRectangle(
      std::vector<glm::vec3> vertices);

  /// RitterEigenSphere
  /// Computes the bounding circle of a set of vertices.
  /// The center of the circle is always at (0, 0), regardless of the input data.
  /// \param vertex_num The number of vertices in the input array.
  /// \param vertex_data Pointer to the array of vertex coordinates.
  /// \return A Circle structure representing the bounding circle.
  static details::collision::Circle ComputeBoundingCircle(
      std::vector<glm::vec3> vertices);

  static details::collision::Dop8 ComputeBoundingK8Dop(
      std::vector<glm::vec3> vertices);

  static details::collision::SsLine ComputeBoundingSsLine(
      std::vector<glm::vec3> vertices);

  static details::collision::ConvexHull ComputeBoundingConvexHull(
      std::vector<glm::vec3> vertices);

 private:
  /// functions for computing bounding circle. Main idea borrowed from:
  /// "Real-Time Collision Detection Christer Ericson"
  static float Variance(std::vector<glm::vec3> vertices);

  static void CovarianceMatrix(glm::mat2 &cov, std::vector<glm::vec3> vertices);

  static void Jacobi(glm::mat2 &a, glm::mat2 &v);

  static void SymSchur2(
      glm::mat2 &a, int p, int q, float &c, float &s);

  static void ExtremePointsAlongDirection(
      glm::vec2 dir, std::vector<glm::vec3> vertices, int *imin, int *imax);

  static void EigenCircle(
      details::collision::Circle &eigCircle, std::vector<glm::vec3> vertices);

  static void CircleOfCircleAndPt(details::collision::Circle &s, glm::vec2 p);

  Type type_;
  std::unique_ptr<uint8_t[]> data_;
  details::collision::Aabb aabb_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_COLLISION_COLLISIONAREA_H_
