#ifndef CGRAYTRACER_OBJECT_H
#define CGRAYTRACER_OBJECT_H

#include "../Material.h"
#include "../Ray.h"
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

  Material getMaterial() const;

  virtual std::optional<double> checkIntersection(Ray ray) const = 0;
};

#endif //CGRAYTRACER_OBJECT_H
