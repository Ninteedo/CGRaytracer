#ifndef CGRAYTRACER_CAMERA_H
#define CGRAYTRACER_CAMERA_H

#include <memory>
#include "Vector3D.h"
#include "JSONParser.h"

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
};

#endif //CGRAYTRACER_CAMERA_H
