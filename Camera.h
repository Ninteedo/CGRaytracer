#ifndef CGRAYTRACER_CAMERA_H
#define CGRAYTRACER_CAMERA_H

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
    Camera(Vector3D position, Vector3D lookAt, Vector3D upVector, double fieldOfView, double exposure, int width, int height);
    Camera(JsonObject cameraJson);
    ~Camera();
};

#endif //CGRAYTRACER_CAMERA_H
