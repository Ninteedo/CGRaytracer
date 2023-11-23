#include "Material.h"
#include "Lambertian.h"
#include "MetalMaterial.h"
#include "GlassMaterial.h"

#include <utility>

Material::Material(double ks, double kd, int specularExponent,
                   Colour diffuseColour, Colour specularColour,
                   bool isReflective, double reflectivity, bool isRefractive,
                   double refractiveIndex, double roughness, const Image *texture)
    : ks(ks), kd(kd), specularExponent(specularExponent),
      diffuseColour(std::move(diffuseColour)), specularColour(std::move(specularColour)),
      isReflective(isReflective), reflectivity(reflectivity),
      isRefractive(isRefractive), refractiveIndex(refractiveIndex), roughness(roughness), texture(texture) {}

Material::Material(JsonObject materialJson)
    : Material(
    materialJson["ks"].asDouble(),
    materialJson["kd"].asDouble(),
    materialJson["specularexponent"].asInt(),
    Colour(materialJson["diffusecolor"].asArray()),
    Colour(materialJson["specularcolor"].asArray()),
    materialJson["isreflective"].asBool(),
    materialJson["reflectivity"].asDouble(),
    materialJson["isrefractive"].asBool(),
    materialJson["refractiveindex"].asDouble(),
    getOrDefault(materialJson, "roughness", JsonValue(0.0)).asDouble()
) {
  if (materialJson.contains("texture")) {
    std::string texturePath = materialJson["texture"].asString();
    texture = Image::loadFromFile(texturePath);
  }
  type = LAMBERTIAN;
  if (materialJson.contains("type")) {
    if (materialJson["type"].asString() == "metal") {
      type = METAL;
    } else if (materialJson["type"].asString() == "glass") {
      type = GLASS;
    }
  }
}

Material::~Material() = default;

Material *Material::fromJson(JsonObject materialJson) {
  if (!materialJson.contains("type") || materialJson["type"].asString() == "lambertian") {
    return new Lambertian(materialJson);
  } else if (materialJson["type"].asString() == "metal") {
    return new MetalMaterial(materialJson);
  } else if (materialJson["type"].asString() == "glass") {
    return new GlassMaterial(materialJson);
  } else {
    throw std::runtime_error("Unknown material type: " + materialJson["type"].asString());
  }
}

double Material::getKs() const { return ks; }

double Material::getKd() const { return kd; }

int Material::getSpecularExponent() const { return specularExponent; }

Colour Material::getDiffuseColour() const { return diffuseColour; }

Colour Material::getSpecularColour() const { return specularColour; }

bool Material::getIsReflective() const { return isReflective; }

double Material::getReflectivity() const { return reflectivity; }

bool Material::getIsRefractive() const { return isRefractive; }

double Material::getRefractiveIndex() const { return refractiveIndex; }

double Material::getRoughness() const { return roughness; }

const Image *Material::getTexture() const {
  return texture;
}

bool Material::isTextured() const {
  return texture != nullptr;
}

MaterialType Material::getType() const {
  return type;
}
