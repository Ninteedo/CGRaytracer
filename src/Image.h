#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include <string>
#include <vector>

#include "Colour.h"

class Image {
 public:

  Image(int width, int height);
  Image(const std::string &filename);
  ~Image();

  [[nodiscard]] Colour getColor(int x, int y) const;
  void setColor(int x, int y, const Colour &colour);

  [[nodiscard]] int getWidth() const;
  [[nodiscard]] int getHeight() const;

  void saveToPPM(const std::string &filename) const;

 private:
  int width;
  int height;
  std::vector<std::shared_ptr<Colour>> pixels;

  [[nodiscard]] bool isInside(int x, int y) const;
};

#endif // IMAGE_H
