#pragma once
#ifndef CGRAYTRACER_OBJECT_H
#define CGRAYTRACER_OBJECT_H

#include "../Material.h"
#include "../Ray.h"
#include "../Interval.h"
#include "../AABB.h"
#include "../Vector2D.h"
#include <memory>
#include <optional>

class Shape {
 private:
  const Material material;

 public:
  explicit Shape(const Material &material);

  // Create a new Shape from JSON.
  // The class corresponds to the type field in the JSON.
  static std::shared_ptr<Shape> fromJson(JsonObject json);

  [[nodiscard]] Material getMaterial() const;

  [[nodiscard]] virtual std::optional<double> checkIntersection(Ray ray, Interval interval) const = 0;

  [[nodiscard]] virtual Vector3D getSurfaceNormal(Vector3D point) const = 0;

  [[nodiscard]] virtual Vector3D getCentroid() const = 0;

  [[nodiscard]] virtual AABB getAABB() const = 0;

  [[nodiscard]] virtual std::optional<Vector2D> getUVCoordinates(Vector3D point) const = 0;
};

#endif //CGRAYTRACER_OBJECT_H
