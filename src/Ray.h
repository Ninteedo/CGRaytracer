#pragma once
#ifndef RAY_H
#define RAY_H

#include "Vector3D.h"

class Ray {
 private:
  const Vector3D origin; // Origin of the ray (A)
  const Vector3D direction; // Direction of the ray (B)

 public:
  // Parameterized constructor
  Ray(const Vector3D &origin, const Vector3D &direction);

  // Copy constructor
  Ray(const Ray &other);

  // Destructor
  ~Ray();

  // Getter for origin
  [[nodiscard]] Vector3D getOrigin() const;

  // Getter for direction
  [[nodiscard]] Vector3D getDirection() const;

  // Function to compute P(t) = A + tB
  [[nodiscard]] Vector3D at(double t) const;
};

#endif // RAY_H
