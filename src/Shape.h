#ifndef CGRAYTRACER_OBJECT_H
#define CGRAYTRACER_OBJECT_H

#include "Material.h"
#include "Ray.h"
#include <memory>

class Shape {
 private:
  const Material material;

 public:
  Shape(const Material material);

  // Create a new Shape from JSON.
  // The class corresponds to the type field in the JSON.
  static std::unique_ptr<Shape> fromJson(JsonObject json);

  Material getMaterial() const;

  virtual bool checkIntersection(const Ray ray, double t) const = 0;
};

#endif //CGRAYTRACER_OBJECT_H
