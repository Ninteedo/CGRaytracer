#include <algorithm>
#include <optional>
#include "GlassMaterial.h"

GlassMaterial::GlassMaterial(JsonObject materialJson) : Material(materialJson) {
  refractiveIndex = materialJson["refractiveindex"].asDouble();
  transmittanceColor = Colour(materialJson["transmittancecolor"].asArray());
  type = GLASS;
}

double fresnelSchlick(double cosTheta, double refractiveIndex) {
  double R0 = (1.0 - refractiveIndex) / (1.0 + refractiveIndex);
  R0 = R0 * R0;
  return 0 + (1.0 - R0) * pow((1.0 - cosTheta), 5.0);
}

std::pair<Colour, Vector3D> GlassMaterial::evaluateTransmission(const Vector3D &incident, const Vector3D &normal) const {
  double eta = normal.dot(incident) > 0.0 ? refractiveIndex : 1.0 / refractiveIndex;
  Vector3D refractDir = incident.refract(normal, eta).normalize();
  return {transmittanceColor, refractDir}; // Color and direction of the refracted ray
}

Colour GlassMaterial::evaluateReflection(const Vector3D &incident, const Vector3D &normal) const {
  Vector3D reflectDir = incident.reflect(normal).normalize();
  double cosTheta = std::abs(incident.dot(normal));
  double fresnelReflectance = fresnelSchlick(cosTheta, refractiveIndex);
  return Colour(fresnelReflectance, fresnelReflectance, fresnelReflectance); // Reflectance color
}

Colour GlassMaterial::evaluateBRDF(const Vector3D &incident,
                                   const Vector3D &outgoing,
                                   const Vector3D &normal,
                                   std::optional<Vector2D> uv) const {
  double r = reflectivity(incident, normal);

  // Return a color representing the proportion of light that's reflected.
  // It can be interpreted as the 'weight' of the reflection.
  return Colour(r, r, r);
}

double GlassMaterial::reflectivity(const Vector3D &incident, const Vector3D &normal) const {
  double cosTheta = std::abs(incident.dot(normal));
  return fresnelSchlick(cosTheta, refractiveIndex);
}
