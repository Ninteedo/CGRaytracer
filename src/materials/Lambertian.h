#ifndef CGRAYTRACER_SRC_MATERIALS_LAMBERTIAN_H_
#define CGRAYTRACER_SRC_MATERIALS_LAMBERTIAN_H_

#include "Material.h"

class Lambertian : public Material {
 public:
  explicit Lambertian(JsonObject materialJson);

  [[nodiscard]] Colour evaluateBRDF(const Vector3D &incident, const Vector3D &outgoing, const Vector3D &normal) const override;
};

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
const Lambertian DEFAULT_MATERIAL = Lambertian(DEFAULT_MATERIAL_JSON.asObject());

#endif //CGRAYTRACER_SRC_MATERIALS_LAMBERTIAN_H_
