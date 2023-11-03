#ifndef CGRAYTRACER_LIGHTSOURCE_H
#define CGRAYTRACER_LIGHTSOURCE_H

#include <memory>
#include "Vector3D.h"
#include "JSONParser.h"

class LightSource {
 private:

 public:

  // Create a new LightSource from JSON.
  // The class corresponds to the type field in the JSON.
  static std::unique_ptr<LightSource> fromJson(JsonObject json);
};

#endif //CGRAYTRACER_LIGHTSOURCE_H
