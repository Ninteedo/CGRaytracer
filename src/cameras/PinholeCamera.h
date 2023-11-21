#pragma once
#ifndef CGRAYTRACER_SRC_PINHOLECAMERA_H_
#define CGRAYTRACER_SRC_PINHOLECAMERA_H_

#include "Camera.h"

class PinholeCamera : public Camera {
 public:
  PinholeCamera(const Vector3D &position,
                const Vector3D &lookAt,
                const Vector3D &upVector,
                double fieldOfView,
                double exposure,
                int width,
                int height);
  explicit PinholeCamera(JsonObject cameraJson);
  ~PinholeCamera();

  [[nodiscard]] Ray getRay(double x, double y, double xOffset = 0.5, double yOffset = 0.5) const override;
};

#endif //CGRAYTRACER_SRC_PINHOLECAMERA_H_
