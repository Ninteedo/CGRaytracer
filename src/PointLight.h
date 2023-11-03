#ifndef CGRAYTRACER_POINTLIGHT_H
#define CGRAYTRACER_POINTLIGHT_H

#include "LightSource.h"

class PointLight : public LightSource {
 private:
  Vector3D position;
  Vector3D intensity;

 public:
  PointLight(Vector3D position, Vector3D intensity);

  PointLight(JsonObject json);
};

#endif //CGRAYTRACER_POINTLIGHT_H
