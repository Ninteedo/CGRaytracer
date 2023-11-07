#pragma once
#ifndef CGRAYTRACER_CYLINDER_H
#define CGRAYTRACER_CYLINDER_H

#include "../Vector3D.h"
#include "Shape.h"
#include "../Interval.h"

class Cylinder : public Shape {
 private:
  Vector3D centre;
  Vector3D axis;
  double radius;
  double height;

 public:
  Cylinder(const Vector3D& centre, const Vector3D& axis, double radius, double height,
           const Material& material);
  explicit Cylinder(JsonObject json);

  [[nodiscard]] std::optional<double> checkIntersection(Ray ray, Interval interval) const override;

  [[nodiscard]] Vector3D getSurfaceNormal(Vector3D point) const override;
};

#endif //CGRAYTRACER_CYLINDER_H
