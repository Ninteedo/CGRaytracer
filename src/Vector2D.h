//
// Created by RIchard on 20/11/2023.
//

#ifndef CGRAYTRACER_SRC_SHAPES_VECTOR2D_H_
#define CGRAYTRACER_SRC_SHAPES_VECTOR2D_H_

#include "JSONParser.h"
class Vector2D {
 public:
  double x;
  double y;

  Vector2D(double x, double y);

  Vector2D();

  Vector2D(JsonArray json);

  Vector2D operator+(const Vector2D &v) const;

  Vector2D operator-(const Vector2D &v) const;

  Vector2D operator*(double val) const;

  Vector2D operator/(double val) const;
};

#endif //CGRAYTRACER_SRC_SHAPES_VECTOR2D_H_
