#include "Vector2D.h"

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}
Vector2D::Vector2D() : x(0), y(0) {}
Vector2D::Vector2D(JsonArray json) : Vector2D(json[0].asDouble(), json[1].asDouble()) {}

Vector2D Vector2D::operator+(const Vector2D &v) const {
  return {x + v.x, y + v.y};
}
Vector2D Vector2D::operator-(const Vector2D &v) const {
  return {x - v.x, y - v.y};
}
Vector2D Vector2D::operator*(double val) const {
  return {x * val, y * val};
}
Vector2D Vector2D::operator/(double val) const {
  return {x / val, y / val};
}
