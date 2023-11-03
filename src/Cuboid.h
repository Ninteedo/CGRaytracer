#ifndef CGRAYTRACER_CUBOID_H
#define CGRAYTRACER_CUBOID_H

#include "Vector3D.h"
#include "Object.h"

class Cuboid : public Object {
private:
    Vector3D centre;
    Vector3D axis;
    double width;
    double height;
    double depth;
};

#endif //CGRAYTRACER_CUBOID_H
