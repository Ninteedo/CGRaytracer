#pragma once
#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D {
private:
    double x, y, z;

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

    // Setter methods
    void setX(double val);
    void setY(double val);
    void setZ(double val);

    // Addition
    Vector3D operator+(const Vector3D& v) const;

    // Subtraction
    Vector3D operator-(const Vector3D& v) const;

    // Dot product
    double dot(const Vector3D& v) const;

    // Cross product
    Vector3D cross(const Vector3D& v) const;

    // Magnitude of the vector
    double magnitude() const;

    // Normalize the vector
    void normalize();

    // Print vector
    void print() const;
};

#endif // VECTOR3D_H
