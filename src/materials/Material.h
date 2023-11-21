#pragma once
#ifndef CGRAYTRACER_MATERIAL_H
#define CGRAYTRACER_MATERIAL_H

#include "../Colour.h"
#include "../JSONParser.h"
#include "../Image.h"

class Material {
 protected:
  const double ks;
  const double kd;
  const int specularExponent;
  const Colour diffuseColour;
  const Colour specularColour;
  const bool isReflective;
  const double reflectivity;
  const bool isRefractive;
  const double refractiveIndex;
  const double roughness;

  const Image *texture = nullptr;

 public:
  Material(double ks,
           double kd,
           int specularExponent,
           Colour diffuseColour,
           Colour specularColour,
           bool isReflective,
           double reflectivity,
           bool isRefractive,
           double refractiveIndex,
           double roughness,
           const Image *texture = nullptr);
  explicit Material(JsonObject materialJson);
  ~Material();

  static Material* fromJson(JsonObject materialJson);

  [[nodiscard]] double getKs() const;
  [[nodiscard]] double getKd() const;
  [[nodiscard]] int getSpecularExponent() const;
  [[nodiscard]] Colour getDiffuseColour() const;
  [[nodiscard]] Colour getSpecularColour() const;
  [[nodiscard]] bool getIsReflective() const;
  [[nodiscard]] double getReflectivity() const;
  [[nodiscard]] bool getIsRefractive() const;
  [[nodiscard]] double getRefractiveIndex() const;
  [[nodiscard]] double getRoughness() const;
  [[nodiscard]] const Image *getTexture() const;

  [[nodiscard]] bool isTextured() const;

  [[nodiscard]] virtual Colour evaluateBRDF(const Vector3D &incident, const Vector3D &outgoing, const Vector3D &normal) const = 0;
};

#endif //CGRAYTRACER_MATERIAL_H
