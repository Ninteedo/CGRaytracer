#pragma once
#ifndef CGRAYTRACER_LIGHTSOURCE_H
#define CGRAYTRACER_LIGHTSOURCE_H

#include <memory>
#include "../Vector3D.h"
#include "../JSONParser.h"
#include "../Colour.h"

class LightSource {
 private:
  Vector3D position;
  Colour intensity;

 public:
  LightSource(const Vector3D &position, Colour intensity);

  // Create a new LightSource from JSON.
  // The class corresponds to the type field in the JSON.
  static std::unique_ptr<LightSource> fromJson(JsonObject json);

  [[nodiscard]] Vector3D getPosition() const;
  [[nodiscard]] Colour getIntensity() const;

  [[nodiscard]] virtual std::pair<Vector3D, double> getDirectionAndDistance(const Vector3D &point) const = 0;

  int samplingFactor = 1;
};

#endif //CGRAYTRACER_LIGHTSOURCE_H
