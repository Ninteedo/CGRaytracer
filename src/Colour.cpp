#include "Colour.h"

#include <utility>
#include <algorithm>

Colour::Colour(double red, double green, double blue) : Vector3D(red, green, blue) {}

Colour::Colour(JsonArray json) : Vector3D(std::move(json)) {}

Colour::Colour(const Vector3D &v) : Colour(v.getX(), v.getY(), v.getZ()) {}

double Colour::red() const {
  return getX();
}

double Colour::green() const {
  return getY();
}

double Colour::blue() const {
  return getZ();
}

int Colour::getRed() const {
  return static_cast<int>(red() * 255);
}

int Colour::getGreen() const {
  return static_cast<int>(green() * 255);
}

int Colour::getBlue() const {
  return static_cast<int>(blue() * 255);
}

std::string Colour::toPPM() const {
  return std::to_string(linear_to_gamma(getRed())) + " "
      + std::to_string(linear_to_gamma(getGreen())) + " "
      + std::to_string(linear_to_gamma(getBlue()));
}

Colour Colour::clamp() const {
  return Colour(std::clamp(red(), 0.0, 1.0),
                std::clamp(green(), 0.0, 1.0),
                std::clamp(blue(), 0.0, 1.0));
}

inline double linear_to_gamma(double val) {
//  return sqrt(val);
  return val;
}
