#include "Cuboid.h"

Cuboid::Cuboid(Vector3D centre, Vector3D axis, Vector3D dimensions,
               Material material)
    : Object(material), centre(centre), axis(axis), dimensions(dimensions) {}

Cuboid::Cuboid(JsonObject json)
    : Object(Material(json["material"].asObject())),
      centre(Vector3D(json["center"].asArray())),
      axis(Vector3D(json["axis"].asArray())),
      dimensions(Vector3D(json["dimensions"].asArray())) {}
