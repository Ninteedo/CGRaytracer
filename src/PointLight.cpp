#include "PointLight.h"

PointLight::PointLight(const Vector3D &position, const Vector3D &intensity)
    : position(position), intensity(intensity) {}

PointLight::PointLight(JsonObject json)
    : position(Vector3D(json["position"].asArray())), intensity(Vector3D(json["intensity"].asArray())) {}
