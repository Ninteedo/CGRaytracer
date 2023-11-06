#include "Shape.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Triangle.h"

Shape::Shape(const Material &material) : material(material) {}

std::shared_ptr<Shape> Shape::fromJson(JsonObject json) {
  std::string type = json["type"].asString();
  if (type == "sphere") {
    return std::make_unique<Sphere>(json);
  } else if (type == "triangle") {
    return std::make_unique<Triangle>(json);
  } else if (type == "cylinder") {
    return std::make_unique<Cylinder>(json);
  } else {
    throw std::runtime_error("Unknown object type: " + type);
  }
}
Material Shape::getMaterial() const {
  return material;
}
