#pragma once
#ifndef CGRAYTRACER_SRC_INTERVAL_H_
#define CGRAYTRACER_SRC_INTERVAL_H_

#include <limits>

class Interval {
 private:
  double min, max;

 public:
  Interval(double _min, double _max) : min(_min), max(_max) {}

  [[nodiscard]] double getMin() const { return min; }
  [[nodiscard]] double getMax() const { return max; }

  [[nodiscard]] bool contains(double val) const;

  [[nodiscard]] bool surrounds(double val) const;
};

const static Interval EMPTY_INTERVAL = Interval(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const static Interval UNIVERSE_INTERVAL = Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

#endif //CGRAYTRACER_SRC_INTERVAL_H_
