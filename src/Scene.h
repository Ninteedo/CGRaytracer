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
#include <utility>

enum RenderMode {
  BINARY,
  PHONG,
  PATHTRACER,
};

struct SampleRecord {
  Colour colour;
  double distance = 0;
};

class Node {
 public:
  std::shared_ptr<Shape> shape;
  AABB aabb;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;

  Node(std::shared_ptr<Shape> shape, AABB aabb, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
      : shape(std::move(shape)), aabb(std::move(aabb)), left(std::move(left)), right(std::move(right)) {}

};

class Scene {
 private:
  int nBounces;
  RenderMode renderMode;
  Camera camera;
  Colour backgroundColour;
  std::vector<std::shared_ptr<Shape>> shapes;
  std::vector<std::shared_ptr<LightSource>> lightSources;
  std::unique_ptr<Node> bspTree;

  void buildBspTree();
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

  Colour sample(int x, int y, int nSamples, const std::function<Colour(const Scene &,
                                                                                         const Ray &)> &sampleFunction);

  Colour sampleShaded(const Ray &ray, int depth = 0);
  Colour sampleDiffuse(const Ray &ray, int depth = 0);
  Colour sampleDiffuseAndSpecular(const Ray &ray, int depth = 0);
  Colour sampleBlinnPhong(const Ray &ray, int depth = 0);
  SampleRecord samplePathtracer(const Ray &ray, int depth = 0);

  [[nodiscard]] std::optional<std::pair<std::shared_ptr<Shape>, double>> checkIntersection(
      const Ray &ray, Interval interval = POSITIVE_INTERVAL) const;
  std::optional<std::pair<std::shared_ptr<Shape>, double>> checkIntersectionNode(const Node *node,
                                                                                 const Ray &ray,
                                                                                 Interval interval) const;

  void addShape(const std::shared_ptr<Shape>& shape);

  [[nodiscard]] bool isInShadow(const Vector3D &point, const LightSource &light) const;
  [[nodiscard]] bool isInShadow(const Ray &shadowRay, double maxDistance, const LightSource &light) const;

  static void printProgress(int current,
                            int total,
                            std::chrono::milliseconds elapsed = std::chrono::milliseconds(-1));
  static std::unique_ptr<Node> buildBspTree(std::vector<std::shared_ptr<Shape>>::iterator begin,
                                     std::vector<std::shared_ptr<Shape>>::iterator end);
};

#endif //CGRAYTRACER_SCENE_H
