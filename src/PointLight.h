#pragma once
#ifndef CGRAYTRACER_POINTLIGHT_H
#define CGRAYTRACER_POINTLIGHT_H

#include "LightSource.h"

class PointLight : public LightSource {
 private:

 public:
  PointLight(const Vector3D &position, const Vector3D &intensity);

  explicit PointLight(JsonObject json);
};

#endif //CGRAYTRACER_POINTLIGHT_H
