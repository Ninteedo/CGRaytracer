#include "Triangle.h"
#include "../Interval.h"

Triangle::Triangle(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2, const Material& material, const Vector2D& uv0, const Vector2D& uv1, const Vector2D& uv2)
    : v0(v0), v1(v1), v2(v2), uv0(uv0), uv1(uv1), uv2(uv2), Shape(material) {}

Triangle::Triangle(JsonObject json)
    : Triangle(
    Vector3D(json["v0"].asArray()),
    Vector3D(json["v1"].asArray()),
    Vector3D(json["v2"].asArray()),
    Material(json["material"].asObject()),
    Vector2D(getOrDefault(json, "uv0", JsonValue(JsonArray(std::vector<JsonValue>{JsonValue(0.0), JsonValue(0.0), JsonValue(0.0)}))).asArray()),
    Vector2D(getOrDefault(json, "uv1", JsonValue(JsonArray(std::vector<JsonValue>{JsonValue(0.0), JsonValue(0.0), JsonValue(0.0)}))).asArray()),
    Vector2D(getOrDefault(json, "uv2", JsonValue(JsonArray(std::vector<JsonValue>{JsonValue(0.0), JsonValue(0.0), JsonValue(0.0)}))).asArray())
  ) {}

std::optional<double> Triangle::checkIntersection(Ray ray, Interval interval) const {
  Vector3D e1 = v1 - v0;
  Vector3D e2 = v2 - v0;
  Vector3D h = ray.direction.cross(e2);
  double a = e1.dot(h);
  if (a > -0.00001 && a < 0.00001) {
    return std::nullopt;
  }
  double f = 1 / a;
  Vector3D s = ray.origin - v0;
  double u = f * s.dot(h);
  if (u < 0.0 || u > 1.0) {
    return std::nullopt;
  }
  Vector3D q = s.cross(e1);
  double v = f * ray.direction.dot(q);
  if (v < 0.0 || u + v > 1.0) {
    return std::nullopt;
  }
  double t0 = f * e2.dot(q);
  if (interval.contains(t0)) {
    return t0;
  }
  return std::nullopt;
}

Vector3D Triangle::getSurfaceNormal(Vector3D point) const {
  return (v1 - v0).cross(v2 - v0).normalize();
}

Vector3D Triangle::getCentroid() const {
  return (v0 + v1 + v2) / 3;
}

AABB Triangle::getAABB() const {
  Vector3D min(
      std::min(std::min(v0.getX(), v1.getX()), v2.getX()),
      std::min(std::min(v0.getY(), v1.getY()), v2.getY()),
      std::min(std::min(v0.getZ(), v1.getZ()), v2.getZ())
  );
  Vector3D max(
      std::max(std::max(v0.getX(), v1.getX()), v2.getX()),
      std::max(std::max(v0.getY(), v1.getY()), v2.getY()),
      std::max(std::max(v0.getZ(), v1.getZ()), v2.getZ())
  );
  return {min, max};
}

std::optional<Vector2D> Triangle::getUVCoordinates(Vector3D point) const {
  // Calculate barycentric coordinates for the point
  Vector3D v0v1 = v1 - v0;
  Vector3D v0v2 = v2 - v0;
  Vector3D n = v0v1.cross(v0v2);
  double area = n.magnitude() / 2;

  double a = (v1 - point).cross(v2 - point).magnitude() / (2 * area);
  double b = (v2 - point).cross(v0 - point).magnitude() / (2 * area);
  double c = 1 - a - b;

  // Interpolate UV coordinates
  double u = a * uv0.x + b * uv1.x + c * uv2.x;
  double v = a * uv0.y + b * uv1.y + c * uv2.y;

  return Vector2D(u, v);
}
