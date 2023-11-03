#include <cmath>
#include "Cylinder.h"

Cylinder::Cylinder(const Vector3D &centre, const Vector3D &axis, double radius, double height,
                   const Material &material)
    : Shape(material), centre(centre), axis(axis), radius(radius),
      height(height) {}

Cylinder::Cylinder(JsonObject json)
    : Cylinder(
    Vector3D(json["center"].asArray()),
    Vector3D(json["axis"].asArray()),
    json["radius"].asDouble(),
    json["height"].asDouble(),
    Material(json["material"].asObject())
) {}

std::optional<double> Cylinder::checkIntersection(const Ray ray) const {
  Vector3D oc = ray.getOrigin() - centre;
  double a = ray.getDirection().dot(ray.getDirection()) - pow(ray.getDirection().dot(axis), 2);
  double b = 2.0 * (oc.dot(ray.getDirection()) - oc.dot(axis) * ray.getDirection().dot(axis));
  double c = oc.dot(oc) - pow(oc.dot(axis), 2) - radius * radius;
  double discriminant = b * b - 4 * a * c;

  if (discriminant >= 0) {
    double t1 = (-b - sqrt(discriminant)) / (2.0 * a);
    double t2 = (-b + sqrt(discriminant)) / (2.0 * a);

    double t = (t1 > 0.00001) ? t1 : t2;
    if (t > 0.00001 && t < height) {
      return t;
    }
  }
  return std::nullopt;
}

Vector3D Cylinder::getSurfaceNormal(Vector3D point) const {
  // TODO: this is probably wrong
  Vector3D projection = axis * (point.dot(axis) / axis.dot(axis));
  Vector3D normal = (point - projection - centre) / radius;
  return normal;
}
