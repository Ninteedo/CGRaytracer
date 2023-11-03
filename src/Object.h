#ifndef CGRAYTRACER_OBJECT_H
#define CGRAYTRACER_OBJECT_H

#include "Material.h"
#include "Ray.h"

class Object {
private:
    const Material material;

public:
    Object(const Material material);

    virtual bool checkIntersection(const Ray ray, double t) const;
};

#endif //CGRAYTRACER_OBJECT_H
