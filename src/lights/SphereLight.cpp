#include "SphereLight.h"
SphereLight::SphereLight(const Vector3D &position, const Vector3D &intensity, double radius) : LightSource(position, intensity), radius(radius) {}

SphereLight::SphereLight(JsonObject json) : SphereLight(Vector3D(json["position"].asArray()), Vector3D(json["intensity"].asArray()), json["radius"].asDouble()) {}

std::pair<Vector3D, double> SphereLight::getDirectionAndDistance(const Vector3D &point) const {
  // Get random point on sphere's surface (on correct side of hemisphere)
  Vector3D centreDirection = (point - getPosition()).normalize();
  Vector3D randomPoint = getPosition() + centreDirection.randomInHemisphere() * radius;
  Vector3D toLight = randomPoint - point;
  double distance = toLight.magnitude();
  return {toLight.normalize(), distance};
}
