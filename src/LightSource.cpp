#include "LightSource.h"
#include "PointLight.h"

std::unique_ptr<LightSource> LightSource::fromJson(JsonObject json) {
  std::string type = json["type"].asString();
  if (type == "pointlight") {
    return std::make_unique<PointLight>(json);
  } else {
    throw std::runtime_error("Unknown light source type: " + type);
  }
}
