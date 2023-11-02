#pragma once
#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D {
private:
    const double x, y, z;

public:
    // Default constructor
    Vector3D();

    // Parameterized constructor
    Vector3D(double x, double y, double z);

    // Copy constructor
    Vector3D(const Vector3D& v);

    // Destructor
    ~Vector3D();

    // Getter methods

    double getX() const;
    double getY() const;
    double getZ() const;

    double operator[](int i) const;

    // Addition
    Vector3D operator+(const Vector3D& v) const;

    // Subtraction
    Vector3D operator-(const Vector3D& v) const;

    // Negative of the vector
    Vector3D operator-() const;

    // Multiplication
    Vector3D operator*(double val) const;

    // Division
    Vector3D operator/(double val) const;

    // Dot product
    double dot(const Vector3D& v) const;

    // Cross product
    Vector3D cross(const Vector3D& v) const;

    // Magnitude of the vector
    double magnitude() const;

    // Normalize the vector
    Vector3D normalize() const;

    // Print vector
    void print() const;
};

#endif // VECTOR3D_H
