#ifndef CGRAYTRACER_MATERIAL_H
#define CGRAYTRACER_MATERIAL_H

#include "Colour.h"
#include "JSONParser.h"

class Material {
private:
    double ks;
    double kd;
    int specularExponent;
    Colour diffuseColour;
    Colour specularColour;
    bool isReflective;
    double reflectivity;
    bool isRefractive;
    double refractiveIndex;

public:
    Material(double ks, double kd, int specularExponent, Colour diffuseColour, Colour specularColour, bool isReflective, double reflectivity, bool isRefractive, double refractiveIndex);
    Material(JsonObject materialJson);
    ~Material();
};

#endif //CGRAYTRACER_MATERIAL_H
