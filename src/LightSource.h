#pragma once
#ifndef CGRAYTRACER_LIGHTSOURCE_H
#define CGRAYTRACER_LIGHTSOURCE_H

#include <memory>
#include "Vector3D.h"
#include "JSONParser.h"

class LightSource {
 private:
  Vector3D position;
  Vector3D intensity;

 public:
  LightSource(const Vector3D &position, const Vector3D &intensity);

  // Create a new LightSource from JSON.
  // The class corresponds to the type field in the JSON.
  static std::unique_ptr<LightSource> fromJson(JsonObject json);

  [[nodiscard]] Vector3D getPosition() const;
  [[nodiscard]] Vector3D getIntensity() const;
};

#endif //CGRAYTRACER_LIGHTSOURCE_H
