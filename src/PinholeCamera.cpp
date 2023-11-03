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
