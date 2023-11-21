#pragma once
#ifndef CGRAYTRACER_POINTLIGHT_H
#define CGRAYTRACER_POINTLIGHT_H

#include "LightSource.h"

class PointLight : public LightSource {
 private:

 public:
  PointLight(const Vector3D &position, const Colour &intensity);

  explicit PointLight(JsonObject json);

  [[nodiscard]] std::pair<Vector3D, double> getDirectionAndDistance(const Vector3D &point) const override;
};

#endif //CGRAYTRACER_POINTLIGHT_H
