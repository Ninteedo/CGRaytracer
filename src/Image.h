#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include <string>
#include <vector>

#include "Colour.h"

class Image {
 public:

  Image(unsigned int width, unsigned int height);
  ~Image();

  [[nodiscard]] Colour getColor(unsigned int x, unsigned int y) const;
  void setColor(unsigned int x, unsigned int y, const Colour &colour);

  [[nodiscard]] unsigned int getWidth() const;
  [[nodiscard]] unsigned int getHeight() const;

  void saveToPPM(const std::string &filename) const;

 private:
  unsigned int width;
  unsigned int height;
  std::vector<std::shared_ptr<Colour>> pixels;

  [[nodiscard]] bool isInside(unsigned int x, unsigned int y) const;
};

#endif // IMAGE_H
