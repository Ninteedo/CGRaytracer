#include "Sphere.h"
#include <cmath>

Sphere::Sphere(Vector3D centre, double radius, Material material)
    : Shape(material), centre(centre), radius(radius) {}

Sphere::Sphere(JsonObject json)
    : Shape(Material(json["material"].asObject())),
      centre(Vector3D(json["center"].asArray())),
      radius(json["radius"].asDouble()) {}

bool Sphere::checkIntersection(Ray ray, double t) const {
  Vector3D oc = ray.getOrigin() - centre;
  double a = ray.getDirection().dot(ray.getDirection());
  double b = 2.0 * oc.dot(ray.getDirection());
  double c = oc.dot(oc) - radius * radius;
  double discriminant = b * b - 4 * a * c;
  return (discriminant >= 0);
}
