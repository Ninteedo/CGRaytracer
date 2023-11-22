#include "FiniteApertureCamera.h"
#include "../Utility.h"
#include <cmath>

FiniteApertureCamera::FiniteApertureCamera(const Vector3D &position,
                                           const Vector3D &lookAt,
                                           const Vector3D &upVector,
                                           double fieldOfView,
                                           double exposure,
                                           int width,
                                           int height,
                                           double apertureSize,
                                           double focusDistance)
    : Camera(position, lookAt, upVector, fieldOfView, exposure, width, height),
      apertureSize(apertureSize),
      focusDistance(focusDistance) {}

FiniteApertureCamera::FiniteApertureCamera(JsonObject cameraJson)
    : Camera(cameraJson),
      apertureSize(cameraJson["apertureSize"].asDouble()),
      focusDistance(cameraJson["focusDistance"].asDouble()) {}

double FiniteApertureCamera::getApertureSize() const {
  return apertureSize;
}

double FiniteApertureCamera::getFocusDistance() const {
  return focusDistance;
}

Ray FiniteApertureCamera::getRay(double x, double y, double xOffset, double yOffset) const {
  // Step 1: Calculate direction as if it's a pinhole camera
  double xNDC = 1 - ((2 * (x + xOffset)) / (getWidth() - 1));
  double yNDC = 1 - (2 * (y + yOffset)) / getHeight();
  double Px = xNDC * viewPlaneWidth / 2.0;
  double Py = yNDC * viewPlaneHeight / 2.0;
  Vector3D pinholeRayDirection = (-w + u * Px + v * Py).normalize();

  // Calculate the point on the focus plane that this ray intersects
  Vector3D focusPoint = getPosition() + pinholeRayDirection * focusDistance;

  // Step 2: Vary the ray's origin within the aperture
  Vector3D apertureOrigin = getPosition();
  // For simplicity, assume a circular aperture and random sampling
  double randomAngle = random_double(0, 2 * M_PI);
  double randomRadius = std::sqrt(random_double());
  Vector3D randomOffset = u * std::cos(randomAngle) * randomRadius * apertureSize +
      v * std::sin(randomAngle) * randomRadius * apertureSize;
  apertureOrigin += randomOffset;

  // Step 3: Adjust the ray's direction to pass through the focus point
  Vector3D apertureRayDirection = (focusPoint - apertureOrigin).normalize();

  // Step 4: Create and return the ray
  return {apertureOrigin, apertureRayDirection};
}
