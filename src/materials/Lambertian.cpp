#include <optional>
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
    texture = Image::loadFromFile(texturePath);
  }
  type = LAMBERTIAN;
}

Colour Lambertian::evaluateBRDF(const Vector3D &incident,
                                const Vector3D &outgoing,
                                const Vector3D &normal,
                                std::optional<Vector2D> uv) const {
  Colour diffuseBaseColour = getDiffuseColour();
  if (isTextured() && uv.has_value()) {
    diffuseBaseColour = texture->getUVColour(uv.value());
  }

  // Diffuse Contribution
  double lambertian = std::abs(normal.dot(incident));
  Colour diffuseContribution = Colour(diffuseBaseColour * lambertian * getKd());

  // Specular Contribution
  Vector3D halfDirection = (incident + outgoing).normalize();
  double specularCoefficient = pow(std::abs(normal.dot(halfDirection)), getSpecularExponent());
  Colour specularContribution = Colour(getSpecularColour() * specularCoefficient * getKs());

  // Combine contributions
  return Colour(diffuseContribution + specularContribution);
}
