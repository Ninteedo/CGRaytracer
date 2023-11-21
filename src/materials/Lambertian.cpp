#include "Lambertian.h"

Lambertian::Lambertian(JsonObject materialJson) : Material(
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
  if (materialJson.count("texture") > 0) {
    std::string texturePath = materialJson["texture"].asString();
    texture = new Image(texturePath);
  }
}

Colour Lambertian::evaluateBRDF(const Vector3D &incident, const Vector3D &outgoing, const Vector3D &normal) const {
  // Diffuse Contribution
  double lambertian = std::max(normal.dot(incident), 0.0);
  Colour diffuseContribution = Colour(getDiffuseColour() * lambertian * getKd());

  // Specular Contribution
  Vector3D halfDirection = (incident + outgoing).normalize();
  double specularCoefficient = pow(std::max(normal.dot(halfDirection), 0.0), getSpecularExponent());
  Colour specularContribution = Colour(getSpecularColour() * specularCoefficient * getKs());

  // Combine contributions
  return Colour(diffuseContribution + specularContribution);
}
