#ifndef CGRAYTRACER_SRC_PINHOLECAMERA_H_
#define CGRAYTRACER_SRC_PINHOLECAMERA_H_

#include "Camera.h"

class PinholeCamera : public Camera {
 public:
  PinholeCamera(Vector3D position,
                Vector3D lookAt,
                Vector3D upVector,
                double fieldOfView,
                double exposure,
                int width,
                int height);
  explicit PinholeCamera(JsonObject cameraJson);
  ~PinholeCamera();
};

#endif //CGRAYTRACER_SRC_PINHOLECAMERA_H_
