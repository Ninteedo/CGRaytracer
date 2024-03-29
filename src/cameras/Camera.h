#pragma once
#ifndef CGRAYTRACER_CAMERA_H
#define CGRAYTRACER_CAMERA_H

#include <memory>
#include "../Vector3D.h"
#include "../JSONParser.h"
#include "../Ray.h"

class Camera {
 private:
  Vector3D position;
  Vector3D lookAt;
  Vector3D upVector;
  double fieldOfView;
  double exposure;
  int width;
  int height;

 protected:
  // precomputed values
  Vector3D w, u, v;
  double viewPlaneWidth, viewPlaneHeight;

 public:
  Camera(const Vector3D &position,
         const Vector3D &lookAt,
         const Vector3D &upVector,
         double fieldOfView,
         double exposure,
         int width,
         int height);
  explicit Camera(JsonObject cameraJson);
  ~Camera();

  static Camera* fromJson(JsonObject json);

  // Getters

  [[nodiscard]] Vector3D getPosition() const;
  [[nodiscard]] Vector3D getLookAt() const;
  [[nodiscard]] Vector3D getUpVector() const;
  [[nodiscard]] double getFieldOfView() const;
  [[nodiscard]] double getExposure() const;
  [[nodiscard]] int getWidth() const;
  [[nodiscard]] int getHeight() const;

  [[nodiscard]] virtual Ray getRay(double x, double y, double xOffset, double yOffset) const = 0;
};

#endif //CGRAYTRACER_CAMERA_H
