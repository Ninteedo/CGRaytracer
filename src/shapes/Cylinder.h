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
  Material *topMaterial, *bottomMaterial;

 public:
  Cylinder(const Vector3D& centre, const Vector3D& axis, double radius, double height,
           Material *material, Material *topMaterial, Material *bottomMaterial);
  explicit Cylinder(JsonObject json);

  [[nodiscard]] std::optional<double> checkIntersection(Ray ray, Interval interval) const override;

  [[nodiscard]] Vector3D getSurfaceNormal(Vector3D point) const override;

  [[nodiscard]] Vector3D getCentroid() const override;

  [[nodiscard]] AABB getAABB() const override;

  [[nodiscard]] std::optional<Vector2D> getUVCoordinates(Vector3D point) const override;
};

#endif //CGRAYTRACER_CYLINDER_H
