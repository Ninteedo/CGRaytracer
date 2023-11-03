#ifndef CGRAYTRACER_CYLINDER_H
#define CGRAYTRACER_CYLINDER_H

#include "Vector3D.h"
#include "Shape.h"

class Cylinder : public Shape {
 private:
  Vector3D centre;
  Vector3D axis;
  double radius;
  double height;

 public:
  Cylinder(Vector3D centre, Vector3D axis, double radius, double height,
           Material material);
  Cylinder(JsonObject json);

  bool checkIntersection(Ray ray, double t) const override;
};

#endif //CGRAYTRACER_CYLINDER_H
