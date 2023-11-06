#pragma once

#include <cmath>
#include <limits>
#include <utility>
#include <memory>
#include <random>

// https://raytracing.github.io/books/RayTracingInOneWeekend.html#antialiasing/somerandomnumberutilities

inline double degrees_to_radians(double degrees) {
  return degrees * M_PI / 180.0;
}

// Returns a random real in [0,1).
inline double random_double() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

// Returns a random real in [min,max).
inline double random_double(double min, double max) {
  return min + (max - min) * random_double();
}
