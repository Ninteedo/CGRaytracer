#include "PointLight.h"

PointLight::PointLight(const Vector3D &position, const Vector3D &intensity)
    : LightSource(position, intensity) {}

PointLight::PointLight(JsonObject json)
    : PointLight(Vector3D(json["position"].asArray()), Vector3D(json["intensity"].asArray())) {}

std::pair<Vector3D, double> PointLight::getDirectionAndDistance(const Vector3D &point) const {
  Vector3D direction = getPosition() - point;
  double distance = direction.magnitudeSquared();
  return {direction / distance, distance};
}
