#include "Triangle.h"

Triangle::Triangle(Vector3D v0, Vector3D v1, Vector3D v2, Material material)
    : v0(v0), v1(v1), v2(v2), Shape(material) {}

Triangle::Triangle(JsonObject json)
    : v0(Vector3D(json["v0"].asArray())),
      v1(Vector3D(json["v1"].asArray())),
      v2(Vector3D(json["v2"].asArray())),
      Shape(Material(json["material"].asObject())) {}

std::optional<double> Triangle::checkIntersection(const Ray ray) const {
  Vector3D e1 = v1 - v0;
  Vector3D e2 = v2 - v0;
  Vector3D h = ray.getDirection().cross(e2);
  double a = e1.dot(h);
  if (a > -0.00001 && a < 0.00001) {
    return std::nullopt;
  }
  double f = 1 / a;
  Vector3D s = ray.getOrigin() - v0;
  double u = f * s.dot(h);
  if (u < 0.0 || u > 1.0) {
    return std::nullopt;
  }
  Vector3D q = s.cross(e1);
  double v = f * ray.getDirection().dot(q);
  if (v < 0.0 || u + v > 1.0) {
    return std::nullopt;
  }
  double t0 = f * e2.dot(q);
  if (t0 > 0.00001) {
    return t0;
  }
  return std::nullopt;
}

Vector3D Triangle::getSurfaceNormal(Vector3D point) const {
  return (v1 - v0).cross(v2 - v0);
}
