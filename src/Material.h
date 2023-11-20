#pragma once
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
  const double roughness;

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
           double roughness);
  explicit Material(JsonObject materialJson);
  ~Material();

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
};

const Material DEFAULT_MATERIAL = Material(0.0, 1.0, 1, Colour(0, 0, 0), Colour(0, 0, 0), false, 0.0, false, 0.0, 0.0);
const JsonValue DEFAULT_MATERIAL_JSON = JsonValue(JsonObject(std::map<std::string, JsonValue> {
  {"ks", JsonValue(0.0)},
  {"kd", JsonValue(1.0)},
  {"specularexponent", JsonValue(1)},
  {"diffusecolor", JsonValue(JsonArray(std::vector<JsonValue> {
      JsonValue(0.0),
      JsonValue(0.0),
      JsonValue(0.0)
  }))},
  {"specularcolor", JsonValue(JsonArray(std::vector<JsonValue> {
      JsonValue(0.0),
      JsonValue(0.0),
      JsonValue(0.0)
  }))},
  {"isreflective", JsonValue(false)},
  {"reflectivity", JsonValue(0.0)},
  {"isrefractive", JsonValue(false)},
  {"refractiveindex", JsonValue(0.0)},
  {"roughness", JsonValue(0.0)}
  }));

#endif //CGRAYTRACER_MATERIAL_H
