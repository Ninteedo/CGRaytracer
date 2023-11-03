#ifndef CGRAYTRACER_SPHERE_H
#define CGRAYTRACER_SPHERE_H

#include "Vector3D.h"
#include "Object.h"

class Sphere : public Object {
 private:
  Vector3D centre;
  double radius;
 public:
  Sphere(Vector3D centre, double radius, Material material);
  Sphere(JsonObject json);

  bool checkIntersection(Ray ray, double t) const override;
};

#endif //CGRAYTRACER_SPHERE_H
