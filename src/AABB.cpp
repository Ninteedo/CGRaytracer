//
// Created by RIchard on 16/11/2023.
//

#include "AABB.h"

AABB::AABB(Vector3D min, Vector3D max) : min(min), max(max) {}

Vector3D AABB::getMin() const {
  return min;
}

Vector3D AABB::getMax() const {
  return max;
}

bool AABB::inVolume(const Vector3D& point) const {
  return (point.getX() >= min.getX() && point.getX() <= max.getX()) &&
      (point.getY() >= min.getY() && point.getY() <= max.getY()) &&
      (point.getZ() >= min.getZ() && point.getZ() <= max.getZ());
}

bool AABB::intersects(const Ray& ray, Interval interval) const {
  double tmin = -std::numeric_limits<double>::infinity();
  double tmax = std::numeric_limits<double>::infinity();

  // Check each axis
  for (int i = 0; i < 3; ++i) {
    if (std::abs(ray.getDirection()[i]) < std::numeric_limits<double>::epsilon()) {
      // Ray is parallel to the slab. No hit if origin not within slab
      if (ray.getOrigin()[i] < min[i] || ray.getOrigin()[i] > max[i]) return false;
    } else {
      double t1 = (min[i] - ray.getOrigin()[i]) / ray.getDirection()[i];
      double t2 = (max[i] - ray.getOrigin()[i]) / ray.getDirection()[i];

      if (t1 > t2) std::swap(t1, t2);
      tmin = std::max(tmin, t1);
      tmax = std::min(tmax, t2);

      if (tmin > tmax) return false;
    }
  }

  return (tmin < interval.getMax() && tmax > interval.getMin());
}

Vector3D AABB::getCentroid() const {
  return (min + max) / 2;
}

AABB AABB::encapsulate(const AABB &other) const {
    return {
        Vector3D(
            std::min(min.getX(), other.getMin().getX()),
            std::min(min.getY(), other.getMin().getY()),
            std::min(min.getZ(), other.getMin().getZ())
        ),
        Vector3D(
            std::max(max.getX(), other.getMax().getX()),
            std::max(max.getY(), other.getMax().getY()),
            std::max(max.getZ(), other.getMax().getZ())
        )
    };
}
