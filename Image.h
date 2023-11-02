#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include "Colour.h"

class Image {
public:

    Image(unsigned int width, unsigned int height);
    ~Image();

    Colour getColor(unsigned int x, unsigned int y) const;
    void setColor(unsigned int x, unsigned int y, const Colour& colour);

    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void saveToPPM(const std::string& filename) const;

private:
    unsigned int width;
    unsigned int height;
    Colour* pixels;

    bool isInside(unsigned int x, unsigned int y) const;
};

#endif // IMAGE_H