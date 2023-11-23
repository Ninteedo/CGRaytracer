#include <algorithm>
#include "MetalMaterial.h"

MetalMaterial::MetalMaterial(JsonObject materialJson) : Material(materialJson) {
  baseColor = Colour(materialJson["basecolor"].asArray());
  roughness = materialJson["roughness"].asDouble();
  fresnelReflectance = materialJson["fresnelreflectance"].asDouble();
  type = METAL;
}

Colour fresnelSchlick(const Vector3D &incident, const Vector3D &normal, const Colour &baseColor, double fresnelReflectance) {
  double cosTheta = std::clamp(incident.dot(normal), 0.0, 1.0);
  return Colour(baseColor + (Colour(1.0, 1.0, 1.0) - baseColor) * pow(1.0 - cosTheta, 5.0));
}

double ggxTrowbridgeReitz(const Vector3D &normal, const Vector3D &incident, double roughness) {
  // Ensure there is a minimum roughness to avoid division by zero
  double alpha = std::max(roughness * roughness, 1e-4); // Replace 1e-4 with a suitable small value

  double cosTheta = std::max(incident.dot(normal), 0.0);
  double denominator = cosTheta * cosTheta * (alpha * alpha - 1.0) + 1.0;

  // Avoid division by zero when cosTheta is zero
  if (denominator == 0) {
    return 0;
  }

  return (alpha * alpha) / (M_PI * denominator * denominator);
}
double ggxLambda(const Vector3D &normal, const Vector3D &v, double roughness) {
  double cosTheta = std::abs(normal.dot(v));
  double sinTheta = sqrt(std::abs(1.0 - cosTheta * cosTheta));
  double alpha = roughness * roughness;
  double alpha2Tan2Theta = (alpha * alpha) * (sinTheta * sinTheta);
  return (-1.0 + sqrt(1.0 + alpha2Tan2Theta)) / 2.0;
}

double smithGeometry(const Vector3D &normal, const Vector3D &incident, const Vector3D &outgoing, double roughness) {
  double lambdaIncident = ggxLambda(normal, incident, roughness);
  double lambdaOutgoing = ggxLambda(normal, outgoing, roughness);
  return 1.0 / (1.0 + lambdaIncident + lambdaOutgoing);
}

Colour MetalMaterial::evaluateBRDF(const Vector3D &incident,
                                   const Vector3D &outgoing,
                                   const Vector3D &normal,
                                   std::optional<Vector2D> uv) const {
  // Fresnel term calculation (using Schlick's approximation or a more complex method)
  Colour F = fresnelSchlick(incident, normal, baseColor, fresnelReflectance);

  // Normal Distribution Function (NDF) for microfacet roughness (using GGX/Trowbridge-Reitz, for example)
  double D = 1;  // ggxTrowbridgeReitz(normal, incident, roughness);

  // Geometry function (Smith's method, for example, to account for shadowing/masking)
  double G = smithGeometry(normal, incident, outgoing, roughness);

  // Specular BRDF formula
  Colour specularBRDF = Colour((F * D * G) / (std::abs(incident.dot(normal)) * std::abs(outgoing.dot(normal))));

  return specularBRDF;
}
