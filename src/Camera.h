#ifndef CGRAYTRACER_CAMERA_H
#define CGRAYTRACER_CAMERA_H

#include <memory>
#include "Vector3D.h"
#include "JSONParser.h"
#include "Ray.h"

class Camera {
 private:
  Vector3D position;
  Vector3D lookAt;
  Vector3D upVector;
  double fieldOfView;
  double exposure;
  int width;
  int height;

 public:
  Camera(Vector3D position,
         Vector3D lookAt,
         Vector3D upVector,
         double fieldOfView,
         double exposure,
         int width,
         int height);
  explicit Camera(JsonObject cameraJson);
  ~Camera();

  std::unique_ptr<Camera> fromJson(JsonObject json);

  // Getters

  Vector3D getPosition() const;
  Vector3D getLookAt() const;
  Vector3D getUpVector() const;
  double getFieldOfView() const;
  double getExposure() const;
  int getWidth() const;
  int getHeight() const;

  Ray getRay(double x, double y) const;
};

#endif //CGRAYTRACER_CAMERA_H
