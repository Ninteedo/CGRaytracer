#include "Vector3D.h"
#include <iostream>
#include <cmath>

Vector3D::Vector3D() : x(0), y(0), z(0) {}

Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3D::Vector3D(const Vector3D &v) = default;

Vector3D::Vector3D(JsonArray json) : x(json[0].asDouble()), y(json[1].asDouble()), z(json[2].asDouble()) {}

Vector3D::~Vector3D() = default;

double Vector3D::getX() const { return x; }
double Vector3D::getY() const { return y; }
double Vector3D::getZ() const { return z; }

double Vector3D::operator[](int i) const {
  switch (i) {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    default: throw std::out_of_range("Index out of bounds");
  }
}

Vector3D Vector3D::operator+(const Vector3D &v) const {
  return {x + v.x, y + v.y, z + v.z};
}

Vector3D Vector3D::operator-(const Vector3D &v) const {
  return {x - v.x, y - v.y, z - v.z};
}

Vector3D Vector3D::operator-() const {
  return {-x, -y, -z};
}

Vector3D Vector3D::operator*(double val) const {
  return {x * val, y * val, z * val};
}

Vector3D Vector3D::operator*(const Vector3D &v) const {
  return {x * v.x, y * v.y, z * v.z};
}

Vector3D Vector3D::operator/(double val) const {
  return {x / val, y / val, z / val};
}

Vector3D Vector3D::operator+=(const Vector3D &v) {
  x += v.x;
  y += v.y;
  z += v.z;

  return *this;
}

double Vector3D::dot(const Vector3D &v) const {
  return x * v.x + y * v.y + z * v.z;
}

Vector3D Vector3D::cross(const Vector3D &v) const {
  return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
}

double Vector3D::magnitude() const {
  return sqrt(x * x + y * y + z * z);
}

double Vector3D::magnitudeSquared() const {
  return x * x + y * y + z * z;
}

Vector3D Vector3D::normalize() const {
  const double mag = magnitude();
  return {x / mag, y / mag, z / mag};
}

void Vector3D::print() const {
  std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}

Vector3D Vector3D::random() {
  return {random_double(), random_double(), random_double()};
}

Vector3D Vector3D::random(double min, double max) {
  return {random_double(min, max), random_double(min, max), random_double(min, max)};
}

Vector3D Vector3D::randomInUnitSphere() {
  while (true) {
    Vector3D p = Vector3D::random(-1, 1);
    if (p.magnitudeSquared() >= 1) continue;
    return p;
  }
}

Vector3D Vector3D::randomUnitVector() {
  return randomInUnitSphere().normalize();
}

Vector3D Vector3D::randomInHemisphere() const {
  Vector3D inUnitSphere = randomInUnitSphere();
  if (dot(inUnitSphere) > 0.0) {
    return inUnitSphere;
  } else {
    return -inUnitSphere;
  }
}

Vector3D Vector3D::reflect(const Vector3D &normal) const {
  return *this - normal * 2 * dot(normal);
}

Vector3D Vector3D::refract(const Vector3D &incident, const Vector3D &normal, double refractiveFactor) {
  double cosTheta = fmin(-incident.dot(normal), 1.0);
  double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

  if (refractiveFactor * sinTheta > 1.0) {
    return reflect(normal);
  }

  Vector3D r_out_perp = (incident + normal * cosTheta) * refractiveFactor;
  Vector3D r_out_parallel = normal * -sqrt(fabs(1.0 - r_out_perp.magnitudeSquared()));
  return r_out_perp + r_out_parallel;
}

std::ostream &operator<<(std::ostream &os, const Vector3D &v) {
  os << "(" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ")";
  return os;
}
