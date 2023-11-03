#include "Image.h"

#include <fstream>
#include <stdexcept>
#include <sstream>

Image::Image(unsigned int width, unsigned int height)
    : width(width), height(height) {
  pixels.resize(width * height);
}

Image::~Image() = default;

Colour Image::getColor(unsigned int x, unsigned int y) const {
  if (!isInside(x, y)) {
    throw std::out_of_range("Coordinates are out of range");
  }
  return *pixels[y * width + x];
}

void Image::setColor(unsigned int x, unsigned int y, const Colour &colour) {
  if (!isInside(x, y)) {
    throw std::out_of_range("Coordinates are out of range");
  }
  pixels[y * width + x] = std::make_shared<Colour>(colour);
}

unsigned int Image::getWidth() const {
  return width;
}

unsigned int Image::getHeight() const {
  return height;
}

bool Image::isInside(unsigned int x, unsigned int y) const {
  return x < width && y < height;
}

void Image::saveToPPM(const std::string &filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file for writing");
  }

  // PPM header
  file << "P3\n";                      // P3 means this is a RGB color image in ASCII
  file << width << " " << height << "\n"; // Image dimensions
  file << "255\n";                      // Maximum color value

  // Image data
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      file << getColor(x, y).toPPM() << "\t";
    }
    file << "\n";
  }

  file.close();
}
