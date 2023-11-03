#ifndef CGRAYTRACER_CUBOID_H
#define CGRAYTRACER_CUBOID_H

#include "Shape.h"
#include "../Vector3D.h"

class Cuboid : public Shape {
 private:
  Vector3D centre;
  Vector3D axis;
  Vector3D dimensions;

 public:
  Cuboid(Vector3D centre, Vector3D axis, Vector3D dimensions,
         Material material);
  Cuboid(JsonObject json);

//  bool checkIntersection(Ray ray, double t) const override;
};

#endif // CGRAYTRACER_CUBOID_H
