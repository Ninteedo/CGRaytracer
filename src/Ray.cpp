#include "Ray.h"

// Parameterized constructor
Ray::Ray(const Vector3D &origin, const Vector3D &direction)
    : origin(origin), direction(direction) {}

// Copy constructor
Ray::Ray(const Ray &other) = default;

// Destructor
Ray::~Ray() = default;

// Function to compute P(t) = A + tB
Vector3D Ray::at(double t) const {
  return origin + direction * t;
}
