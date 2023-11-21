#ifndef CGRAYTRACER_SRC_CAMERAS_FINITEAPERTURECAMERA_H_
#define CGRAYTRACER_SRC_CAMERAS_FINITEAPERTURECAMERA_H_

#include "Camera.h"

class FiniteApertureCamera : public Camera {
 private:
  double apertureSize; // Size of the aperture
  double focusDistance; // Distance at which the camera is focused

 public:
  FiniteApertureCamera(const Vector3D &position,
                       const Vector3D &lookAt,
                       const Vector3D &upVector,
                       double fieldOfView,
                       double exposure,
                       int width,
                       int height,
                       double apertureSize,
                       double focusDistance);

  explicit FiniteApertureCamera(JsonObject cameraJson);

  [[nodiscard]] double getApertureSize() const;

  [[nodiscard]] double getFocusDistance() const;

  // Override the getRay method
  [[nodiscard]] Ray getRay(double x, double y, double xOffset = 0.5, double yOffset = 0.5) const override;
};

#endif //CGRAYTRACER_SRC_CAMERAS_FINITEAPERTURECAMERA_H_
