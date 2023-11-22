#ifndef CGRAYTRACER_SRC_MATERIALS_METALMATERIAL_H_
#define CGRAYTRACER_SRC_MATERIALS_METALMATERIAL_H_

#include "Material.h"

class MetalMaterial : public Material {
 private:
  Colour baseColor;        // Inherent color of the metal
  double roughness;        // Surface roughness
  double fresnelReflectance; // Fresnel reflectance at normal incidence

 public:
  explicit MetalMaterial(JsonObject materialJson);

  [[nodiscard]] Colour evaluateBRDF(const Vector3D &incident, const Vector3D &outgoing, const Vector3D &normal) const override;
};
#endif //CGRAYTRACER_SRC_MATERIALS_METALMATERIAL_H_
