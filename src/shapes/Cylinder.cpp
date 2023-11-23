#include <cmath>
#include <iostream>
#include "Cylinder.h"
#include "../materials/Lambertian.h"
#include "../Utility.h"

Cylinder::Cylinder(const Vector3D &centre, const Vector3D &axis, double radius, double height,
                   Material *material, Material *topMaterial, Material *bottomMaterial)
    : Shape(material), centre(centre), axis(axis.normalize()), radius(radius),
      height(height), topMaterial(topMaterial), bottomMaterial(bottomMaterial) {}

Cylinder::Cylinder(JsonObject json)
    : Cylinder(
    Vector3D(json["center"].asArray()),
    Vector3D(json["axis"].asArray()),
    json["radius"].asDouble(),
    json["height"].asDouble(),
    Material::fromJson(getOrDefault(json, "material", DEFAULT_MATERIAL_JSON).asObject()),
    Material::fromJson(getOrDefault(json, "topmaterial", DEFAULT_MATERIAL_JSON).asObject()),
    Material::fromJson(getOrDefault(json, "bottommaterial", DEFAULT_MATERIAL_JSON).asObject())
) {}

std::optional<double> Cylinder::checkIntersection(Ray ray, Interval interval) const {
  Vector3D oc = ray.origin - centre;
  Vector3D d = ray.direction;
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
      Vector3D intersectionPoint = ray.origin + ray.direction * t;
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
  double tBottomCap = ((centre - axis * height) - ray.origin).dot(axis) / d.dot(axis);
  // Intersection with the plane of the bottom cap
  if (interval.contains(tBottomCap)) {
    Vector3D pBottomCap = ray.origin + d * tBottomCap;
    // Vector from the center of the bottom cap to the intersection point
    Vector3D vBottomCap = pBottomCap - (centre - axis * height);
    if (vBottomCap.magnitudeSquared() <= radiusSq && (!result || tBottomCap < result.value())) {
      result = tBottomCap;
    }
  }

  // Check the top cap
  double tTopCap = ((centre + axis * height) - ray.origin).dot(axis) / d.dot(axis);
  // Intersection with the plane of the top cap
  if (interval.contains(tTopCap)) {
    Vector3D pTopCap = ray.origin + d * tTopCap;
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

AABB Cylinder::getAABB() const {
  Vector3D p1 = centre - axis * height;
  Vector3D p2 = centre + axis * height;

  Vector3D minPoint(
      std::min(p1.getX(), p2.getX()) - radius,
      std::min(p1.getY(), p2.getY()) - radius,
      std::min(p1.getZ(), p2.getZ()) - radius
  );
  Vector3D maxPoint(
      std::max(p1.getX(), p2.getX()) + radius,
      std::max(p1.getY(), p2.getY()) + radius,
      std::max(p1.getZ(), p2.getZ()) + radius
  );

  return {minPoint, maxPoint};
}

std::optional<Vector2D> Cylinder::getUVCoordinates(Vector3D point) const {
  Vector3D localPoint = point - centre;  // Translate to the cylinder's local space

  // Determine if the point is on the top or bottom cap
  double heightAlongAxis = localPoint.dot(axis);
  if (std::abs(heightAlongAxis - height) < EPSILON) { // Top cap
    double u = (localPoint.x / (radius + 1)) / 2;
    double v = (localPoint.z / (radius + 1)) / 2;
    u = u < 0 ? u + 1 : u;  // Normalize U
    v = v < 0 ? v + 1 : v;  // Normalize V
    return Vector2D(u, v);
  } else if (std::abs(heightAlongAxis + height) < EPSILON) { // Bottom cap
    double u = (localPoint.x / (radius + 1)) / 2;
    double v = (localPoint.z / (radius + 1)) / 2;
    u = u < 0 ? u + 1 : u;  // Normalize U
    v = v < 0 ? v + 1 : v;  // Normalize V
    return Vector2D(u, v);
  } else { // Curved surface
    double u = atan2(localPoint.z, localPoint.x) / (2 * M_PI);
    u = u < 0 ? u + 1 : u;  // Normalize U
    double v = 1 - ((heightAlongAxis + height) / (2 * height)); // Normalize V
    return Vector2D(u, v);
  }
}
