#ifndef COLOUR_H
#define COLOUR_H

#include <string>
#include "Vector3D.h"

class Colour : public Vector3D
{
public:
    // Constructor
    Colour(double red = 0, double green = 0, double blue = 0);

    Colour(JsonArray json);

    double red() const;
    double green() const;
    double blue() const;

    int getRed() const;
    int getGreen() const;
    int getBlue() const;

    std::string toPPM() const;
};

#endif // COLOUR_H
