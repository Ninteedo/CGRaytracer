#ifndef CGRAYTRACER_SCENE_H
#define CGRAYTRACER_SCENE_H

#include "JSONParser.h"
#include "Colour.h"
#include "Object.h"
#include "LightSource.h"
#include "Camera.h"
#include "Image.h"

enum RenderMode {
  BINARY,
  PHONG,
  PATHTRACER,
};

class Scene {
 private:
  Colour colour;
  Camera camera;
  std::vector<Object> objects;
  std::vector<LightSource> lightSources;
  int nBounces;
  RenderMode renderMode;
 public:
  Scene(Colour colour,
        Camera camera,
        std::vector<Object> objects,
        std::vector<LightSource> lightSources,
        int nBounces,
        RenderMode renderMode);
  Scene(JsonObject sceneJson);
  ~Scene();

  Image render();
};

#endif //CGRAYTRACER_SCENE_H
