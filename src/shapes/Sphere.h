#pragma once
#ifndef CGRAYTRACER_SPHERE_H
#define CGRAYTRACER_SPHERE_H

#include "../Vector3D.h"
#include "Shape.h"
#include "../Interval.h"

class Sphere : public Shape {
 private:
  Vector3D centre;
  double radius;
 public:
  Sphere(const Vector3D& centre, double radius, const Material& material);
  explicit Sphere(JsonObject json);

  [[nodiscard]] std::optional<double> checkIntersection(Ray ray, Interval interval) const override;

  [[nodiscard]] Vector3D getSurfaceNormal(Vector3D point) const override;

  [[nodiscard]] Vector3D getCentroid() const override;
};

#endif //CGRAYTRACER_SPHERE_H
