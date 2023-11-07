#include <cmath>
#include "Cylinder.h"
#include "../Interval.h"

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

std::optional<double> Cylinder::checkIntersection(Ray ray, Interval interval) const {
  Vector3D oc = ray.getOrigin() - centre;
  Vector3D d = ray.getDirection();
  Vector3D axisDir = axis; // Ensure this is a unit vector
  double radiusSq = radius * radius;

  // Coefficients for the quadratic equation at^2 + bt + c = 0
  double a = d.cross(axisDir).magnitudeSquared();
  double b = 2.0 * (d.cross(axisDir)).dot(oc.cross(axisDir));
  double c = oc.cross(axisDir).magnitudeSquared() - radiusSq;

  // Solve the quadratic equation for t
  double discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return std::nullopt; // No intersection, the discriminant is negative.
  }

  // Compute the two points of intersection t1 and t2
  double t0 = (-b - sqrt(discriminant)) / (2.0 * a);
  double t1 = (-b + sqrt(discriminant)) / (2.0 * a);

  // Check each intersection point for validity
  std::optional<double> result = std::nullopt;
  for (double t : {t0, t1}) {
    if (!interval.contains(t)) continue;

    // Check the height bounds
    Vector3D intersectionPoint = ray.getOrigin() + ray.getDirection() * t;
    Vector3D baseToIntersection = intersectionPoint - centre;
    double heightAlongAxis = baseToIntersection.dot(axisDir);

    if (heightAlongAxis < 0 || heightAlongAxis > this->height) continue; // Outside the height bounds of the cylinder

    // If we reach this point, the intersection is valid.
    // You can either choose to return the first valid t or choose the smallest positive t.
    if (!result || t < result) {
      result = t;
    }
  }

  return result;
}

Vector3D Cylinder::getSurfaceNormal(Vector3D point) const {
  // TODO: this is probably wrong
  Vector3D projection = axis * (point.dot(axis) / axis.dot(axis));
  Vector3D normal = (point - projection - centre) / radius;
  return normal;
}
