#include "PointLight.h"

PointLight::PointLight(const Vector3D &position, const Vector3D &intensity)
    : LightSource(position, intensity) {}

PointLight::PointLight(JsonObject json)
    : PointLight(Vector3D(json["position"].asArray()), Vector3D(json["intensity"].asArray())) {}
