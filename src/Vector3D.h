#pragma once
#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "JSONParser.h"

class Vector3D {
 private:
  double x, y, z;

 public:
  // Default constructor
  Vector3D();

  // Parameterized constructor
  Vector3D(double x, double y, double z);

  // Copy constructor
  Vector3D(const Vector3D &v);

  explicit Vector3D(JsonArray json);

  // Destructor
  ~Vector3D();

  // Getter methods

  [[nodiscard]] double getX() const;
  [[nodiscard]] double getY() const;
  [[nodiscard]] double getZ() const;

  double operator[](int i) const;

  // Addition
  Vector3D operator+(const Vector3D &v) const;

  // Subtraction
  Vector3D operator-(const Vector3D &v) const;

  // Negative of the vector
  Vector3D operator-() const;

  // Multiplication
  Vector3D operator*(double val) const;

  // Division
  Vector3D operator/(double val) const;

  // Dot product
  [[nodiscard]] double dot(const Vector3D &v) const;

  // Cross product
  [[nodiscard]] Vector3D cross(const Vector3D &v) const;

  // Magnitude of the vector
  [[nodiscard]] double magnitude() const;

  // Squared magnitude of the vector
  [[nodiscard]] double magnitudeSquared() const;

  // Normalize the vector
  [[nodiscard]] Vector3D normalize() const;

  // Print vector
  void print() const;
};

#endif // VECTOR3D_H
