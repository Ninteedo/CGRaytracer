#include "Material.h"

Material::Material(double ks, double kd, int specularExponent,
                   Colour diffuseColour, Colour specularColour,
                   bool isReflective, double reflectivity, bool isRefractive,
                   double refractiveIndex)
    : ks(ks), kd(kd), specularExponent(specularExponent),
      diffuseColour(diffuseColour), specularColour(specularColour),
      isReflective(isReflective), reflectivity(reflectivity),
      isRefractive(reflectivity), refractiveIndex(refractiveIndex) {}

Material::Material(JsonObject materialJson)
    : ks(materialJson["ks"].asDouble()), kd(materialJson["kd"].asDouble()),
      specularExponent(materialJson["specularexponent"].asInt()),
      diffuseColour(Colour(materialJson["diffusecolor"].asArray())),
      specularColour(Colour(materialJson["specularcolor"].asArray())),
      isReflective(materialJson["isreflective"].asBool()),
      reflectivity(materialJson["reflectivity"].asDouble()),
      isRefractive(materialJson["isrefractive"].asBool()),
      refractiveIndex(materialJson["refractiveindex"].asDouble()) {}

Material::~Material() = default;

double Material::getKs() const { return ks; }

double Material::getKd() const { return kd; }

int Material::getSpecularExponent() const { return specularExponent; }

Colour Material::getDiffuseColour() const { return diffuseColour; }

Colour Material::getSpecularColour() const { return specularColour; }

bool Material::getIsReflective() const { return isReflective; }

double Material::getReflectivity() const { return reflectivity; }

bool Material::getIsRefractive() const { return isRefractive; }

double Material::getRefractiveIndex() const { return refractiveIndex; }
