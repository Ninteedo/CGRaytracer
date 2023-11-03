#ifndef CGRAYTRACER_SCENE_H
#define CGRAYTRACER_SCENE_H

#include "JSONParser.h"
#include "Colour.h"
#include "Shape.h"
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
  int nBounces;
  RenderMode renderMode;
  Camera camera;
  Colour colour;
  std::vector<std::unique_ptr<Shape>> objects;
  std::vector<std::unique_ptr<LightSource>> lightSources;
 public:
  Scene(Colour colour,
        Camera camera,
        std::vector<Shape> objects,
        std::vector<LightSource> lightSources,
        int nBounces,
        RenderMode renderMode);
  Scene(JsonObject sceneJson);
  ~Scene() = default;

  static Scene loadFromFile(const std::string& filename);

  Image render();
  Image renderBinary();

  struct Intersection {
    bool hit;
    double t;
  };

  Intersection checkIntersection(Ray ray, double t) const {
    for (int i = 0; i < objects.size(); i++) {
      if (objects[i]->checkIntersection(ray, t)) {
        return {true, t};
      }
    }
    return {false, t};
  }
};

#endif //CGRAYTRACER_SCENE_H
