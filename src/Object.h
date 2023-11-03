#ifndef CGRAYTRACER_OBJECT_H
#define CGRAYTRACER_OBJECT_H

#include "Material.h"
#include "Ray.h"
#include <memory>

class Object {
 private:
  const Material material;

 public:
  Object(const Material material);

  // Create a new Object from JSON.
  // The class corresponds to the type field in the JSON.
  static std::unique_ptr<Object> fromJson(JsonObject json);

  virtual bool checkIntersection(const Ray ray, double t) const = 0;
};

#endif //CGRAYTRACER_OBJECT_H
