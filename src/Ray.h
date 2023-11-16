#pragma once
#ifndef RAY_H
#define RAY_H

#include "Vector3D.h"

class Ray {
 public:
  const Vector3D origin; // Origin of the ray (A)
  const Vector3D direction; // Direction of the ray (B)

  // Parameterized constructor
  Ray(const Vector3D &origin, const Vector3D &direction);

  // Copy constructor
  Ray(const Ray &other);

  // Destructor
  ~Ray();

  // Function to compute P(t) = A + tB
  [[nodiscard]] Vector3D at(double t) const;
};

#endif // RAY_H
