#pragma once
#ifndef CGRAYTRACER_SCENE_H
#define CGRAYTRACER_SCENE_H

#include "JSONParser.h"
#include "Colour.h"
#include "shapes/Shape.h"
#include "LightSource.h"
#include "Camera.h"
#include "Image.h"
#include <random>
#include <functional>

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
  std::vector<std::shared_ptr<Shape>> shapes;
  std::vector<std::shared_ptr<LightSource>> lightSources;
 public:
  Scene(Colour colour,
        const Camera &camera,
        std::vector<std::shared_ptr<Shape>> objects,
        std::vector<std::shared_ptr<LightSource>> lightSources,
        int nBounces,
        RenderMode renderMode);
  explicit Scene(JsonObject sceneJson);
  ~Scene() = default;

  static Scene loadFromFile(const std::string &filename);

  Image render();
  Image renderBinary();
  Image renderShaded();

  Colour sample(unsigned int x, unsigned int y, int nSamples, const std::function<Colour(const Scene &, const Ray &)>& sampleFunction);

  Colour sampleShaded(const Ray& ray);

  [[nodiscard]] std::optional<std::pair<std::shared_ptr<Shape>, double>> checkIntersection(const Ray &ray) const;
};

#endif //CGRAYTRACER_SCENE_H
