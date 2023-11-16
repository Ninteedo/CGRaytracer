//
// Created by RIchard on 16/11/2023.
//

#include "AABB.h"

AABB::AABB(const Vector3D &min, const Vector3D &max) : min(min), max(max) {}

bool AABB::inVolume(const Vector3D &point) const {
  return (point.getX() >= min.getX() && point.getX() <= max.getX()) &&
      (point.getY() >= min.getY() && point.getY() <= max.getY()) &&
      (point.getZ() >= min.getZ() && point.getZ() <= max.getZ());
}

constexpr double NEGATIVE_INFINITY = -std::numeric_limits<double>::infinity();
constexpr double POSITIVE_INFINITY = std::numeric_limits<double>::infinity();
constexpr double AUTO_EPSILON = std::numeric_limits<double>::epsilon();

// Check whether a ray intersects the AABB
// This function is called many times and is a bottleneck
bool AABB::intersects(const Ray &ray, Interval interval) const {
  double tMin = NEGATIVE_INFINITY;
  double tMax = POSITIVE_INFINITY;

  for (int i = 0; i < 3; ++i) {
    if (std::abs(ray.direction[i]) < AUTO_EPSILON && (ray.origin[i] < min[i] || ray.origin[i] > max[i])) {
      // Ray is parallel to the slab. No hit if origin not within slab
      return false;
    }
  }

  Vector3D inverseDirection = Vector3D(
      1 / ray.direction.getX(),
      1 / ray.direction.getY(),
      1 / ray.direction.getZ()
  );

  // Check each axis
  for (int i = 0; i < 3; ++i) {
    double t1 = (min[i] - ray.origin[i]) * inverseDirection[i];
    double t2 = (max[i] - ray.origin[i]) * inverseDirection[i];

    if (t1 > t2) std::swap(t1, t2);
    tMin = std::max(tMin, t1);
    tMax = std::min(tMax, t2);

    if (tMin > tMax) return false;
  }

  return (tMin < interval.getMax() && tMax > interval.getMin());
}

Vector3D AABB::getCentroid() const {
  return (min + max) / 2;
}

AABB AABB::encapsulate(const AABB &other) const {
  return {
      Vector3D(
          std::min(min.getX(), other.min.getX()),
          std::min(min.getY(), other.min.getY()),
          std::min(min.getZ(), other.min.getZ())
      ),
      Vector3D(
          std::max(max.getX(), other.max.getX()),
          std::max(max.getY(), other.max.getY()),
          std::max(max.getZ(), other.max.getZ())
      )
  };
}
