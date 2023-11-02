#ifndef CGRAYTRACER_SPHERE_H
#define CGRAYTRACER_SPHERE_H

#include "Vector3D.h"
#include "Object.h"

class Sphere : public Object {
private:
    Vector3D centre;
    double radius;
};

#endif //CGRAYTRACER_SPHERE_H
