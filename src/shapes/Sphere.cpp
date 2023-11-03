#include "Sphere.h"
#include <cmath>

Sphere::Sphere(const Vector3D& centre, double radius, const Material& material)
    : Shape(material), centre(centre), radius(radius) {}

Sphere::Sphere(JsonObject json)
    : Sphere(
    Vector3D(json["center"].asArray()),
    json["radius"].asDouble(),
    Material(json["material"].asObject())
) {}

std::optional<double> Sphere::checkIntersection(const Ray ray) const {
  Vector3D oc = ray.getOrigin() - centre;
  double a = ray.getDirection().dot(ray.getDirection());
  double b = 2.0 * oc.dot(ray.getDirection());
  double c = oc.dot(oc) - radius * radius;
  double discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return std::nullopt;
  } else {
    double t = (-b - sqrt(discriminant)) / (2.0 * a);
    return t;
  }
}
Vector3D Sphere::getSurfaceNormal(Vector3D point) const {
  return (point - centre) / radius;
}
