#include "LightSource.h"

#include <stdexcept>
#include <utility>

#include "PointLight.h"
#include "SphereLight.h"
#include "../Colour.h"

LightSource::LightSource(const Vector3D &position, Colour intensity) : position(position), intensity(std::move(intensity)) {}

std::unique_ptr<LightSource> LightSource::fromJson(JsonObject json) {
  std::string type = json["type"].asString();
  if (type == "pointlight") {
    return std::make_unique<PointLight>(json);
  } else if (type == "spherelight") {
    return std::make_unique<SphereLight>(json);
  } else {
    throw std::runtime_error("Unknown light source type: " + type);
  }
}

Vector3D LightSource::getPosition() const {
  return position;
}

Colour LightSource::getIntensity() const {
  return intensity;
}
