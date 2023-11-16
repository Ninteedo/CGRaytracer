#include <cmath>
#include "Cylinder.h"

Cylinder::Cylinder(const Vector3D &centre, const Vector3D &axis, double radius, double height,
                   const Material &material)
    : Shape(material), centre(centre), axis(axis.normalize()), radius(radius),
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
  double radiusSq = radius * radius;

  // Coefficients for the quadratic equation at^2 + bt + c = 0
  double a = d.cross(axis).magnitudeSquared();
  double half_b = (d.cross(axis)).dot(oc.cross(axis));
  double c = oc.cross(axis).magnitudeSquared() - radiusSq;

  // Solve the quadratic equation for t
  double discriminant = half_b * half_b - a * c;

  std::optional<double> result = std::nullopt;
  if (discriminant >= 0) {

    // Compute the two points of intersection t1 and t2
    double t0 = (-half_b - sqrt(discriminant)) / a;
    double t1 = (-half_b + sqrt(discriminant)) / a;

    // Check each intersection point for validity
    for (double t : {t0, t1}) {
      if (!interval.contains(t)) continue;

      // Check the height bounds
      Vector3D intersectionPoint = ray.getOrigin() + ray.getDirection() * t;
      Vector3D baseToIntersection = intersectionPoint - centre;
      double heightAlongAxis = baseToIntersection.dot(axis);

      // Check if outside the height bounds of the cylinder
      if (heightAlongAxis < -height || heightAlongAxis > height) continue;

      // If we reach this point, the intersection is valid.
      // You can either choose to return the first valid t or choose the smallest positive t.
      if (!result || t < result) {
        result = t;
      }
    }
  }

  // Check the caps
  // Check the bottom cap
  double tBottomCap = ((centre - axis * height) - ray.getOrigin()).dot(axis) / d.dot(axis);
  // Intersection with the plane of the bottom cap
  if (interval.contains(tBottomCap)) {
    Vector3D pBottomCap = ray.getOrigin() + d * tBottomCap;
    // Vector from the center of the bottom cap to the intersection point
    Vector3D vBottomCap = pBottomCap - (centre - axis * height);
    if (vBottomCap.magnitudeSquared() <= radiusSq && (!result || tBottomCap < result.value())) {
      result = tBottomCap;
    }
  }

  // Check the top cap
  double tTopCap = ((centre + axis * height) - ray.getOrigin()).dot(axis) / d.dot(axis);
  // Intersection with the plane of the top cap
  if (interval.contains(tTopCap)) {
    Vector3D pTopCap = ray.getOrigin() + d * tTopCap;
    // Vector from the center of the top cap to the intersection point
    Vector3D vTopCap = pTopCap - (centre + axis * height);
    if (vTopCap.magnitudeSquared() <= radiusSq && (!result || tTopCap < result.value())) {
      result = tTopCap;
    }
  }

  return result;
}

Vector3D Cylinder::getSurfaceNormal(Vector3D point) const {
  // Calculate the vector from the axis origin to the point
  Vector3D vectorFromAxis = point - centre;

  // Project this vector onto the axis to find the closest point on the axis to 'point'
  double t = vectorFromAxis.dot(axis);

  // If the point is on the top cap
  if (std::abs(t - height) < EPSILON) { // EPSILON is a small threshold value
    return axis; // The normal is the same as the cylinder axis for the top cap
  }

  // If the point is on the bottom cap
  if (std::abs(t + height) < EPSILON) { // close to zero
    return -axis; // The normal is the negative of the cylinder axis for the bottom cap
  }

  // If the point is on the curved surface
  Vector3D closestPointOnAxis = centre + axis * t;
  Vector3D normal = point - closestPointOnAxis;
  return normal.normalize();
}

Vector3D Cylinder::getCentroid() const {
  return centre;
}
