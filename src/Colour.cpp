#include "Colour.h"

#include <utility>

Colour::Colour(double red, double green, double blue) : Vector3D(red, green, blue) {}

Colour::Colour(JsonArray json) : Vector3D(std::move(json)) {}

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
  return std::to_string(getRed()) + " " + std::to_string(getGreen()) + " " + std::to_string(getBlue());
}
