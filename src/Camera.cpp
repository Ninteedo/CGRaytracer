#include <cmath>
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

Vector3D Camera::getPosition() const { return position; }

Vector3D Camera::getLookAt() const { return lookAt; }

Vector3D Camera::getUpVector() const { return upVector; }

double Camera::getFieldOfView() const { return fieldOfView; }

double Camera::getExposure() const { return exposure; }

int Camera::getWidth() const { return width; }

int Camera::getHeight() const { return height; }

Ray Camera::getRay(double x, double y) const {
  // Step 1: Establish camera coordinate system
  Vector3D w = (position - lookAt).normalize(); // Assuming looking from position to lookAt
  Vector3D u = upVector.cross(w).normalize();
  Vector3D v = w.cross(u);

  // Step 2: Calculate view plane size based on FOV and aspect ratio
  double aspectRatio = static_cast<double>(width) / height;
  double viewHeight = 2.0 * tan(fieldOfView * 0.5);
  double viewWidth = viewHeight * aspectRatio;

  // Step 3: Calculate direction of the ray for pixel (x, y)
  double xNDC = (2 * x) / width - 1;
  double yNDC = 1 - (2 * y) / height;
  double Px = xNDC * viewWidth / 2.0;
  double Py = yNDC * viewHeight / 2.0;
  Vector3D rayDirection = (-w + u * Px + v * Py).normalize(); // Normalize the direction

  // Step 4: Create and return the ray
  return Ray(position, rayDirection);
}
