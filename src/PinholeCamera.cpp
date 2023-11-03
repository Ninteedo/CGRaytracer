#include "PinholeCamera.h"

PinholeCamera::PinholeCamera(Vector3D position,
                             Vector3D lookAt,
                             Vector3D upVector,
                             double fieldOfView,
                             double exposure,
                             int width,
                             int height)
    : Camera(position, lookAt, upVector, fieldOfView, exposure, width, height) {}

PinholeCamera::PinholeCamera(JsonObject cameraJson) : Camera(cameraJson) {}
