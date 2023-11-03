#ifndef CGRAYTRACER_MATERIAL_H
#define CGRAYTRACER_MATERIAL_H

#include "Colour.h"
#include "JSONParser.h"

class Material {
 private:
  const double ks;
  const double kd;
  const int specularExponent;
  const Colour diffuseColour;
  const Colour specularColour;
  const bool isReflective;
  const double reflectivity;
  const bool isRefractive;
  const double refractiveIndex;

 public:
  Material(double ks,
           double kd,
           int specularExponent,
           Colour diffuseColour,
           Colour specularColour,
           bool isReflective,
           double reflectivity,
           bool isRefractive,
           double refractiveIndex);
  explicit Material(JsonObject materialJson);
  ~Material();

  double getKs() const;
  double getKd() const;
  int getSpecularExponent() const;
  Colour getDiffuseColour() const;
  Colour getSpecularColour() const;
  bool getIsReflective() const;
  double getReflectivity() const;
  bool getIsRefractive() const;
  double getRefractiveIndex() const;
};

#endif //CGRAYTRACER_MATERIAL_H
