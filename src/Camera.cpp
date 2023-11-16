#include <cmath>
#include "Camera.h"
#include "PinholeCamera.h"
#include <stdexcept>

Camera::Camera(const Vector3D &position,
               const Vector3D &lookAt,
               const Vector3D &upVector,
               double fieldOfView,
               double exposure,
               int width,
               int height) : position(position),
                             lookAt(lookAt),
                             upVector(upVector),
                             fieldOfView(fieldOfView),
                             exposure(exposure),
                             width(width),
                             height(height) {
  // Step 1: Establish camera coordinate system
  w = (position - lookAt).normalize(); // Assuming looking from position to lookAt
  u = upVector.cross(w).normalize();
  v = w.cross(u);

  // Step 2: Calculate view plane size based on FOV and aspect ratio
  double aspectRatio = static_cast<double>(width) / height;
  viewPlaneHeight = 2.0 * tan(fieldOfView * 0.5);
  viewPlaneWidth = viewPlaneHeight * aspectRatio;
}

Camera::Camera(JsonObject json)
    : Camera(Vector3D(json["position"].asArray()),
             Vector3D(json["lookAt"].asArray()),
             Vector3D(json["upVector"].asArray()),
             json["fov"].asDouble(),
             json["exposure"].asDouble(),
             json["width"].asInt(),
             json["height"].asInt()) {}

Camera::~Camera() = default;

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

Ray Camera::getRay(double x, double y, double xOffset, double yOffset) const {
  // Step 3: Calculate direction of the ray for pixel (x, y)
  double xNDC = 1 - ((2 * (x + xOffset)) / (width - 1));
  double yNDC = 1 - (2 * (y + yOffset)) / height;
  double Px = xNDC * viewPlaneWidth / 2.0;
  double Py = yNDC * viewPlaneHeight / 2.0;
  Vector3D rayDirection = (-w + u * Px + v * Py).normalize(); // Normalize the direction

  // Step 4: Create and return the ray
  return {position, rayDirection};
}
