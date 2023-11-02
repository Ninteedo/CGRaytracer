#ifndef CGRAYTRACER_TRIANGLE_H
#define CGRAYTRACER_TRIANGLE_H

#include "Vector3D.h"
#include "Object.h"

class Triangle : public Object {
private:
    Vector3D v0;
    Vector3D v1;
    Vector3D v2;
};

#endif //CGRAYTRACER_TRIANGLE_H
