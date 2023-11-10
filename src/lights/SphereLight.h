#ifndef CGRAYTRACER_SRC_LIGHTS_SPHERELIGHT_H_
#define CGRAYTRACER_SRC_LIGHTS_SPHERELIGHT_H_

#include "LightSource.h"

class SphereLight : public LightSource {
 private:
  double radius;

 public:
  SphereLight(const Vector3D &position, const Vector3D &intensity, double radius);

  explicit SphereLight(JsonObject json);

  [[nodiscard]] std::pair<Vector3D, double> getDirectionAndDistance(const Vector3D &point) const override;

  int samplingFactor = 10;
};

#endif //CGRAYTRACER_SRC_LIGHTS_SPHERELIGHT_H_
