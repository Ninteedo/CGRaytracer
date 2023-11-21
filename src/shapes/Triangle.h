#pragma once
#ifndef CGRAYTRACER_TRIANGLE_H
#define CGRAYTRACER_TRIANGLE_H

#include "Shape.h"
#include "../Vector3D.h"

class Triangle : public Shape {
 private:
  Vector3D v0, v1, v2;

  Vector2D uv0, uv1, uv2;  // UV coordinates

 public:
  Triangle(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2, Material *material, const Vector2D& uv0, const Vector2D& uv1, const Vector2D& uv2);
  explicit Triangle(JsonObject json);

  [[nodiscard]] std::optional<double> checkIntersection(Ray ray, Interval interval) const override;

  [[nodiscard]] Vector3D getSurfaceNormal(Vector3D point) const override;

  [[nodiscard]] Vector3D getCentroid() const override;

  [[nodiscard]] AABB getAABB() const override;

  [[nodiscard]] std::optional<Vector2D> getUVCoordinates(Vector3D point) const override;
};

#endif // CGRAYTRACER_TRIANGLE_H
