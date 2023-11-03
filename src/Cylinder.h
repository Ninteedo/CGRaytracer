#ifndef CGRAYTRACER_CYLINDER_H
#define CGRAYTRACER_CYLINDER_H

#include "Vector3D.h"
#include "Object.h"

class Cylinder : public Object {
private:
    Vector3D centre;
    Vector3D axis;
    double radius;
    double height;
};

#endif //CGRAYTRACER_CYLINDER_H
