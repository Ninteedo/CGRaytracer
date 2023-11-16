#ifndef CGRAYTRACER_SRC_AABB_H_
#define CGRAYTRACER_SRC_AABB_H_

#include "Vector3D.h"
#include "Interval.h"
#include "Ray.h"

class AABB {
 private:
  Vector3D min;
  Vector3D max;

 public:
  AABB(Vector3D min, Vector3D max);

  [[nodiscard]] Vector3D getMin() const;
  [[nodiscard]] Vector3D getMax() const;

  [[nodiscard]] bool inVolume(const Vector3D& point) const;

  [[nodiscard]] bool intersects(const Ray& ray, Interval interval) const;
};

#endif //CGRAYTRACER_SRC_AABB_H_
