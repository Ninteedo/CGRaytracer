#ifndef CGRAYTRACER_SRC_MATERIALS_GLASSMATERIAL_H_
#define CGRAYTRACER_SRC_MATERIALS_GLASSMATERIAL_H_

#include "Material.h"

class GlassMaterial : public Material {
 private:
  double refractiveIndex;
  Colour transmittanceColor;

 public:
  explicit GlassMaterial(JsonObject materialJson);

  [[nodiscard]] Colour evaluateBRDF(const Vector3D &incident,
                                    const Vector3D &outgoing,
                                    const Vector3D &normal,
                                    std::optional<Vector2D> uv) const override;

  [[nodiscard]] std::pair<Colour, Vector3D> evaluateTransmission(const Vector3D &incident, const Vector3D &normal) const;

  [[nodiscard]] Colour evaluateReflection(const Vector3D &incident, const Vector3D &normal) const;

  [[nodiscard]] double reflectivity(const Vector3D &incident, const Vector3D &normal) const;
};

#endif //CGRAYTRACER_SRC_MATERIALS_GLASSMATERIAL_H_
