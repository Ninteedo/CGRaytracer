//
// Created by RIchard on 03/11/2023.
//

#include "Interval.h"

bool Interval::contains(double val) const {
  return min <= val && val <= max;
}

bool Interval::surrounds(double val) const {
  return min < val && val < max;
}
