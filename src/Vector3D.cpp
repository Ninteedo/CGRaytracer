#include "Vector3D.h"
#include <iostream>
#include <cmath>

Vector3D::Vector3D() {
  data = _mm256_setzero_pd();
}

Vector3D::Vector3D(double x, double y, double z) {
  data = _mm256_set_pd(0, z, y, x);
}

Vector3D::Vector3D(const Vector3D &v) = default;

Vector3D::Vector3D(const __m256d &data) : data(data) {}

Vector3D::Vector3D(JsonArray json) : Vector3D(json[0].asDouble(), json[1].asDouble(), json[2].asDouble()) {}

Vector3D::~Vector3D() = default;

double Vector3D::getX() const { return _mm256_cvtsd_f64(data); }

double Vector3D::getY() const {
  __m128d high = _mm256_extractf128_pd(data, 0);
  return _mm_cvtsd_f64(_mm_unpackhi_pd(high, high));
}

double Vector3D::getZ() const {
  return _mm_cvtsd_f64(_mm256_extractf128_pd(data, 1));
}

double Vector3D::max() const {
  __m128d high = _mm256_extractf128_pd(data, 0);
  __m128d low = _mm256_castpd256_pd128(data);
  __m128d max = _mm_max_pd(low, high);
  return _mm_cvtsd_f64(_mm_max_pd(max, _mm_shuffle_pd(max, max, 1)));
}

double Vector3D::min() const {
  __m128d high = _mm256_extractf128_pd(data, 0);
  __m128d low = _mm256_castpd256_pd128(data);
  __m128d min = _mm_min_pd(low, high);
  return _mm_cvtsd_f64(_mm_min_pd(min, _mm_shuffle_pd(min, min, 1)));
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
  return _mm256_add_pd(this->data, v.data);
}

Vector3D Vector3D::operator-(const Vector3D &v) const {
  return _mm256_sub_pd(this->data, v.data);
}

Vector3D Vector3D::operator-() const {
  return _mm256_sub_pd(_mm256_setzero_pd(), data);
}

Vector3D Vector3D::operator*(double val) const {
  return _mm256_mul_pd(this->data, _mm256_set1_pd(val));
}

Vector3D Vector3D::operator*(const Vector3D &v) const {
  return _mm256_mul_pd(this->data, v.data);
}

Vector3D Vector3D::operator/(double val) const {
  return _mm256_div_pd(this->data, _mm256_set1_pd(val));
}

Vector3D Vector3D::operator+=(const Vector3D &v) {
  this->data = _mm256_add_pd(this->data, v.data);
  return *this;
}

bool Vector3D::operator==(const Vector3D &v) const {
  return getX() == v.getX() && getY() == v.getY() && getZ() == v.getZ();
}

bool Vector3D::operator!=(const Vector3D &v) const {
  return !(*this == v);
}

double Vector3D::dot(const Vector3D &v) const {
  __m256d mul = _mm256_mul_pd(this->data, v.data);
  __m128d high = _mm256_extractf128_pd(mul, 1);
  __m128d low = _mm256_castpd256_pd128(mul);
  __m128d sum = _mm_add_pd(low, high);
  return _mm_cvtsd_f64(_mm_hadd_pd(sum, sum));
}

Vector3D Vector3D::cross(const Vector3D &v) const {
  return {getY() * v.getZ() - getZ() * v.getY(), getZ() * v.getX() - getX() * v.getZ(), getX() * v.getY() - getY() * v.getX()};
}

double Vector3D::magnitude() const {
  return sqrt(magnitudeSquared());
}

double Vector3D::magnitudeSquared() const {
  return dot(*this);
}

Vector3D Vector3D::normalize() const {
  const double mag = magnitude();
  __m256d scalar = _mm256_set1_pd(mag);
  return _mm256_div_pd(this->data, scalar);
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

Vector3D Vector3D::refract(const Vector3D &normal, double refractiveFactor) const {
  double cosTheta = fmin(-this->dot(normal), 1.0);
  double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

  if (refractiveFactor * sinTheta > 1.0) {
    return reflect(normal);
  }

  Vector3D r_out_perp = (*this + normal * cosTheta) * refractiveFactor;
  Vector3D r_out_parallel = normal * -sqrt(fabs(1.0 - r_out_perp.magnitudeSquared()));
  return r_out_perp + r_out_parallel;
}

std::ostream &operator<<(std::ostream &os, const Vector3D &v) {
  os << "(" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ")";
  return os;
}
