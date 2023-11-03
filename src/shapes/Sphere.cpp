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
  double a = ray.getDirection().magnitudeSquared();
  double half_b = oc.dot(ray.getDirection());
  double c = oc.magnitudeSquared() - radius * radius;
  double discriminant = half_b * half_b - a * c;

  if (discriminant >= 0) {
    return (-half_b - sqrt(discriminant)) / a;
  }
  return std::nullopt;
}

Vector3D Sphere::getSurfaceNormal(Vector3D point) const {
  return (point - centre) / radius;
}
