#pragma once
#ifndef CGRAYTRACER_SRC_UTILITY_H_
#define CGRAYTRACER_SRC_UTILITY_H_

// https://raytracing.github.io/books/RayTracingInOneWeekend.html#antialiasing/somerandomnumberutilities

#include <limits>

const static double EPSILON = std::numeric_limits<double>::epsilon();

double degrees_to_radians(double degrees);

// Returns a random real in [0,1).
double random_double();

// Returns a random real in [min,max).
double random_double(double min, double max);

#endif //CGRAYTRACER_SRC_UTILITY_H_
