#include "Vector3D.h"
#include <iostream>
#include <cmath>

Vector3D::Vector3D() : x(0), y(0), z(0) {}

Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3D::Vector3D(const Vector3D& v) : x(v.x), y(v.y), z(v.z) {}

Vector3D::~Vector3D() {}

double Vector3D::getX() const { return x; }
double Vector3D::getY() const { return y; }
double Vector3D::getZ() const { return z; }

void Vector3D::setX(double val) { x = val; }
void Vector3D::setY(double val) { y = val; }
void Vector3D::setZ(double val) { z = val; }

Vector3D Vector3D::operator+(const Vector3D& v) const {
    return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator-(const Vector3D& v) const {
    return Vector3D(x - v.x, y - v.y, z - v.z);
}

double Vector3D::dot(const Vector3D& v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vector3D Vector3D::cross(const Vector3D& v) const {
    return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

double Vector3D::magnitude() const {
    return sqrt(x * x + y * y + z * z);
}

void Vector3D::normalize() {
    double mag = magnitude();
    if (mag > 0) {
        x /= mag;
        y /= mag;
        z /= mag;
    }
}

void Vector3D::print() const {
    std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}
