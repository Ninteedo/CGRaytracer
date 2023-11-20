#include "Image.h"

#include <fstream>
#include <stdexcept>
#include <sstream>

Image::Image(int width, int height)
    : width(width), height(height) {
  pixels.resize(width * height);
}

Image::Image(const std::string &filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file for reading");
  }

  std::string line;
  std::getline(file, line); // Read the magic number (P3)
  if (line != "P3") {
    throw std::runtime_error("Invalid PPM file format");
  }

  // Skip comments
  while (file.peek() == '#') {
    std::getline(file, line);
  }

  // Read image dimensions
  std::getline(file, line);
  std::stringstream dimensions(line);
  dimensions >> width >> height;
  if (width <= 0 || height <= 0) {
    throw std::runtime_error("Invalid image dimensions");
  }

  // Read and ignore the maximum color value
  std::getline(file, line);

  // Resize the pixels vector
  pixels.resize(width * height);

  // Read the pixel data
  int r, g, b;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      file >> r >> g >> b;
      pixels[y * width + x] = std::make_shared<Colour>(r, g, b);
    }
  }
}

Image::~Image() = default;

Colour Image::getColor(int x, int y) const {
  if (!isInside(x, y)) {
    throw std::out_of_range("Coordinates are out of range");
  }
  return *pixels[y * width + x];
}

void Image::setColor(int x, int y, const Colour &colour) {
  if (!isInside(x, y)) {
    throw std::out_of_range("Coordinates are out of range");
  }
  pixels[y * width + x] = std::make_shared<Colour>(colour);
}

int Image::getWidth() const {
  return width;
}

int Image::getHeight() const {
  return height;
}

bool Image::isInside(int x, int y) const {
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
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      file << getColor(x, y).toPPM() << "\t";
    }
    file << "\n";
  }

  file.close();
}
