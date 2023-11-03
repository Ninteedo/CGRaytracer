#include "Camera.h"
#include "PinholeCamera.h"

Camera::Camera(Vector3D position,
               Vector3D lookAt,
               Vector3D upVector,
               double fieldOfView,
               double exposure,
               int width,
               int height) : position(position),
                             lookAt(lookAt),
                             upVector(upVector),
                             fieldOfView(fieldOfView),
                             exposure(exposure),
                             width(width),
                             height(height) {}

Camera::Camera(JsonObject json)
    : position(Vector3D(json["position"].asArray())),
      lookAt(Vector3D(json["lookAt"].asArray())),
      upVector(Vector3D(json["upVector"].asArray())),
      fieldOfView(json["fov"].asDouble()),
      exposure(json["exposure"].asDouble()),
      width(json["width"].asInt()),
      height(json["height"].asInt()) {}

Camera::~Camera() {}

std::unique_ptr<Camera> Camera::fromJson(JsonObject json) {
  std::string type = json["type"].asString();
  if (type == "pinhole") {
    return std::make_unique<PinholeCamera>(json);
  } else {
    throw std::runtime_error("Unknown camera type: " + type);
  }
}
