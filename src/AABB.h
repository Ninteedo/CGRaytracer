#ifndef CGRAYTRACER_SRC_AABB_H_
#define CGRAYTRACER_SRC_AABB_H_

#include "Vector3D.h"
#include "Interval.h"
#include "Ray.h"

class AABB {
 public:
  Vector3D min;
  Vector3D max;

  AABB(const Vector3D& min, const Vector3D& max);

  [[nodiscard]] bool inVolume(const Vector3D& point) const;

  [[nodiscard]] bool intersects(const Ray& ray, Interval interval) const;

  [[nodiscard]] Vector3D getCentroid() const;

  [[nodiscard]] AABB encapsulate(const AABB& other) const;
};

#endif //CGRAYTRACER_SRC_AABB_H_
