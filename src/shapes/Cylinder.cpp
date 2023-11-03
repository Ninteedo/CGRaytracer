#include <cmath>
#include "Cylinder.h"

Cylinder::Cylinder(Vector3D centre, Vector3D axis, double radius, double height,
                   Material material)
    : Shape(material), centre(centre), axis(axis), radius(radius),
      height(height) {}

Cylinder::Cylinder(JsonObject json) : Shape(Material(json["material"].asObject())),
                                      centre(Vector3D(json["center"].asArray())),
                                      axis(Vector3D(json["axis"].asArray())),
                                      radius(json["radius"].asDouble()),
                                      height(json["height"].asDouble()) {}

bool Cylinder::checkIntersection(Ray ray, double t) const {
    Vector3D oc = ray.getOrigin() - centre;
    double a = ray.getDirection().dot(ray.getDirection()) - pow(ray.getDirection().dot(axis), 2);
    double b = 2.0 * (oc.dot(ray.getDirection()) - oc.dot(axis) * ray.getDirection().dot(axis));
    double c = oc.dot(oc) - pow(oc.dot(axis), 2) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    return (discriminant >= 0);
}
