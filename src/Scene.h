#pragma once
#ifndef CGRAYTRACER_SCENE_H
#define CGRAYTRACER_SCENE_H

#include "JSONParser.h"
#include "Colour.h"
#include "shapes/Shape.h"
#include "lights/LightSource.h"
#include "Camera.h"
#include "Image.h"
#include <random>
#include <functional>
#include <chrono>

enum RenderMode {
  BINARY,
  PHONG,
  PATHTRACER,
};

struct SampleRecord {
  Colour colour;
  double distance = 0;
};

class Scene {
 private:
  int nBounces;
  RenderMode renderMode;
  Camera camera;
  Colour backgroundColour;
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
  Image renderBlinnPhong();
  Image renderPathtracer();

  Colour sample(unsigned int x, unsigned int y, int nSamples, const std::function<Colour(const Scene &,
                                                                                         const Ray &)> &sampleFunction);

  Colour sampleShaded(const Ray &ray, int depth = 0);
  Colour sampleDiffuse(const Ray &ray, int depth = 0);
  Colour sampleDiffuseAndSpecular(const Ray &ray, int depth = 0);
  Colour sampleBlinnPhong(const Ray &ray, int depth = 0);
  SampleRecord samplePathtracer(const Ray &ray, int depth = 0);

  [[nodiscard]] std::optional<std::pair<std::shared_ptr<Shape>, double>> checkIntersection(const Ray &ray,
                                                                                           Interval interval = POSITIVE_INTERVAL) const;

  bool isInShadow(const Vector3D &point, const LightSource &light) const;
  bool isInShadow(const Ray &shadowRay, double maxDistance, const LightSource &light) const;

  static void printProgress(unsigned int current, unsigned int total, std::chrono::milliseconds elapsed = std::chrono::milliseconds(-1)) ;
};

#endif //CGRAYTRACER_SCENE_H
