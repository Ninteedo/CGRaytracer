#include "Sphere.h"
#include <cmath>

Sphere::Sphere(const Vector3D& centre, double radius, Material *material)
    : Shape(material), centre(centre), radius(radius) {}

Sphere::Sphere(JsonObject json)
    : Sphere(
    Vector3D(json["center"].asArray()),
    json["radius"].asDouble(),
    Material::fromJson(json["material"].asObject())
) {}

std::optional<double> Sphere::checkIntersection(Ray ray, Interval interval) const {
  Vector3D oc = ray.origin - centre;
  double a = ray.direction.magnitudeSquared();
  double half_b = oc.dot(ray.direction);
  double c = oc.magnitudeSquared() - radius * radius;
  double discriminant = half_b * half_b - a * c;

  if (discriminant >= 0) {
    double root1 = (-half_b - sqrt(discriminant)) / a;
    double root2 = (-half_b + sqrt(discriminant)) / a;

    if (interval.contains(root1)) {
      return root1;
    } else if (interval.contains(root2)) {
      return root2;
    }
  }
  return std::nullopt;
}

Vector3D Sphere::getSurfaceNormal(Vector3D point) const {
  return (point - centre) / radius;
}

Vector3D Sphere::getCentroid() const {
  return centre;
}

AABB Sphere::getAABB() const {
  return {centre - Vector3D(radius, radius, radius), centre + Vector3D(radius, radius, radius)};
}

std::optional<Vector2D> Sphere::getUVCoordinates(Vector3D point) const {
  Vector3D p = point - centre;  // Translate the point to the sphere's local space
  double u = 0.5 + atan2(p.getZ(), p.getX()) / (2 * M_PI);
  double v = 0.5 - asin(p.getY() / radius) / M_PI;
  return Vector2D(u, v);
}
