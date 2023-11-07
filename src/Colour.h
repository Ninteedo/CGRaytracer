#pragma once
#ifndef COLOUR_H
#define COLOUR_H

#include <string>
#include <cmath>
#include "Vector3D.h"

class Colour : public Vector3D {
 public:
  explicit Colour(double red = 0, double green = 0, double blue = 0);

  explicit Colour(JsonArray json);

  explicit Colour(const Vector3D &v);

  [[nodiscard]] double red() const;
  [[nodiscard]] double green() const;
  [[nodiscard]] double blue() const;

  [[nodiscard]] int getRed() const;
  [[nodiscard]] int getGreen() const;
  [[nodiscard]] int getBlue() const;

  [[nodiscard]] std::string toPPM() const;
};

inline double linear_to_gamma(double val);

#endif // COLOUR_H
