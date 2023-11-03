#ifndef CGRAYTRACER_TRIANGLE_H
#define CGRAYTRACER_TRIANGLE_H

#include "Shape.h"
#include "Vector3D.h"

class Triangle : public Shape {
 private:
  Vector3D v0;
  Vector3D v1;
  Vector3D v2;

 public:
  Triangle(Vector3D v0, Vector3D v1, Vector3D v2, Material material);
  Triangle(JsonObject json);

  bool checkIntersection(Ray ray, double t) const override;
};

#endif // CGRAYTRACER_TRIANGLE_H
