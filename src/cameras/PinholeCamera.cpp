#include "PinholeCamera.h"

#include <utility>

PinholeCamera::PinholeCamera(const Vector3D &position,
                             const Vector3D &lookAt,
                             const Vector3D &upVector,
                             double fieldOfView,
                             double exposure,
                             int width,
                             int height)
    : Camera(position, lookAt, upVector, fieldOfView, exposure, width, height) {}

PinholeCamera::PinholeCamera(JsonObject cameraJson) : Camera(std::move(cameraJson)) {}
PinholeCamera::~PinholeCamera() = default;

Ray PinholeCamera::getRay(double x, double y, double xOffset, double yOffset) const {
  // Step 3: Calculate direction of the ray for pixel (x, y)
  double xNDC = 1 - ((2 * (x + xOffset)) / (getWidth() - 1));
  double yNDC = 1 - (2 * (y + yOffset)) / getHeight();
  double Px = xNDC * viewPlaneWidth / 2.0;
  double Py = yNDC * viewPlaneHeight / 2.0;
  Vector3D rayDirection = (-w + u * Px + v * Py).normalize(); // Normalize the direction

  // Step 4: Create and return the ray
  return {getPosition(), rayDirection};
}
