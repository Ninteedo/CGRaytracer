#include "Vector3D.h"
#include <iostream>
#include <cmath>

Vector3D::Vector3D() : x(0), y(0), z(0) {}

Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3D::Vector3D(const Vector3D &v) = default;

Vector3D::Vector3D(JsonArray json) : Vector3D(json[0].asDouble(), json[1].asDouble(), json[2].asDouble()) {}

Vector3D::~Vector3D() = default;

double Vector3D::getX() const { return x; }

double Vector3D::getY() const {
  return y;
}

double Vector3D::getZ() const {
  return z;
}

double Vector3D::max() const {
  return std::max(std::max(x, y), z);
}

double Vector3D::min() const {
  return std::min(std::min(x, y), z);
}

double Vector3D::operator[](int i) const {
  switch (i) {
    case 0: return getX();
    case 1: return getY();
    case 2: return getZ();
    default: throw std::out_of_range("Index out of bounds");
  }
}

Vector3D Vector3D::operator+(const Vector3D &v) const {
  return {getX() + v.getX(), getY() + v.getY(), getZ() + v.getZ()};
}

Vector3D Vector3D::operator-(const Vector3D &v) const {
  return {getX() - v.getX(), getY() - v.getY(), getZ() - v.getZ()};
}

Vector3D Vector3D::operator-() const {
  return {-getX(), -getY(), -getZ()};
}

Vector3D Vector3D::operator*(double val) const {
  return {getX() * val, getY() * val, getZ() * val};
}

Vector3D Vector3D::operator*(const Vector3D &v) const {
  return {getX() * v.getX(), getY() * v.getY(), getZ() * v.getZ()};
}

Vector3D Vector3D::operator/(double val) const {
  return {getX() / val, getY() / val, getZ() / val};
}

Vector3D Vector3D::operator+=(const Vector3D &v) {
  this->x += v.x;
  this->y += v.y;
  this->z += v.z;
  return *this;
}

bool Vector3D::operator==(const Vector3D &v) const {
  return getX() == v.getX() && getY() == v.getY() && getZ() == v.getZ();
}

bool Vector3D::operator!=(const Vector3D &v) const {
  return !(*this == v);
}

double Vector3D::dot(const Vector3D &v) const {
  return getX() * v.getX() + getY() * v.getY() + getZ() * v.getZ();
}

Vector3D Vector3D::cross(const Vector3D &v) const {
  return {getY() * v.getZ() - getZ() * v.getY(), getZ() * v.getX() - getX() * v.getZ(),
          getX() * v.getY() - getY() * v.getX()};
}

double Vector3D::magnitude() const {
  return sqrt(magnitudeSquared());
}

double Vector3D::magnitudeSquared() const {
  return dot(*this);
}

Vector3D Vector3D::normalize() const {
  const double mag = magnitude();
  return {getX() / mag, getY() / mag, getZ() / mag};
}

void Vector3D::print() const {
  std::cout << "(" << getX() << ", " << getY() << ", " << getZ() << ")" << std::endl;
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

Vector3D Vector3D::reflect(const Vector3D &normal, double roughness) const {
  Vector3D roughnessVector;
  if (roughness > 0.0) {
    roughnessVector = randomInUnitSphere() * roughness;
  }
  return *this - normal * 2 * dot(normal) + roughnessVector;
}

Vector3D Vector3D::refract(const Vector3D &normal, double refractiveIndex) const {
  // refractive factor is incident medium refractive index / refractive medium refractive index
  // use Snell's law to compute the refracted ray
  bool isExitRay = dot(normal) > 0;
  Vector3D properNormal = normal;
  if (isExitRay) {
    properNormal = -normal;
  } else {
    refractiveIndex = 1 / refractiveIndex;
  }

  const double cosI = -dot(properNormal);
  const double sinT2 = refractiveIndex * refractiveIndex * (1.0 - cosI * cosI);
  if(sinT2 > 1.0) return reflect(properNormal); // TIR
  const double cosT = sqrt(1.0 - sinT2);
  return *this * refractiveIndex + properNormal * (refractiveIndex * cosI - cosT);
}

std::ostream &operator<<(std::ostream &os, const Vector3D &v) {
  os << "(" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ")";
  return os;
}
