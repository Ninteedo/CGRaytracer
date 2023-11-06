#include "Scene.h"
#include "Utility.h"

#include <utility>
#include <cmath>
#include <random>
#include <iostream>

Scene::Scene(Colour colour,
             const Camera &camera,
             std::vector<std::shared_ptr<Shape>> objects,
             std::vector<std::shared_ptr<LightSource>> lightSources,
             int nBounces,
             RenderMode renderMode)
    : backgroundColour(std::move(colour)),
      camera(camera),
      shapes(std::move(objects)),
      lightSources(std::move(lightSources)),
      nBounces(nBounces),
      renderMode(renderMode) {}

Scene::Scene(JsonObject json) : nBounces(json["nbounces"].asInt()), camera(json["camera"].asObject()) {
  std::string renderModeString = json["rendermode"].asString();
  if (renderModeString == "binary") {
    renderMode = BINARY;
  } else if (renderModeString == "phong") {
    renderMode = PHONG;
  } else if (renderModeString == "pathtracer") {
    renderMode = PATHTRACER;
  } else {
    throw std::runtime_error("Invalid render mode");
  }
  JsonObject sceneObject = json["scene"].asObject();
  backgroundColour = Colour(sceneObject["backgroundcolor"].asArray());
  JsonArray lightsJson = sceneObject["lightsources"].asArray();
  for (const auto &i : lightsJson) {
    lightSources.push_back(LightSource::fromJson(i.asObject()));
  }
  JsonArray objectsJson = sceneObject["shapes"].asArray();
  for (const auto &i : objectsJson) {
    shapes.push_back(Shape::fromJson(i.asObject()));
  }
}

Scene Scene::loadFromFile(const std::string &filename) {
  JsonParser parser;
  return Scene(parser.parseFile(filename).asObject());
}

Image Scene::render() {
  if (renderMode == BINARY) {
    return renderShaded();
//  } else if (renderMode == PHONG) {
//    return renderPhong();
//  } else if (renderMode == PATHTRACER) {
//    return renderPathtracer();
  } else {
    throw std::runtime_error("Invalid render mode");
  }
}

Image Scene::renderBinary() {
  Image image(camera.getWidth(), camera.getHeight());

  for (unsigned int y = 0; y < camera.getHeight(); y++) {
    for (unsigned int x = 0; x < camera.getWidth(); x++) {
      Ray ray = camera.getRay(x, y);
      std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

      Colour pixelColour;
      if (intersection.has_value()) {
        pixelColour = intersection.value().first->getMaterial().getDiffuseColour();
      } else {
        pixelColour = backgroundColour;
      }

      image.setColor(x, y, pixelColour);
    }
  }
  return image;
}

Image Scene::renderShaded() {
  Image image(camera.getWidth(), camera.getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->sampleDiffuse(r); };

  for (unsigned int y = 0; y < camera.getHeight(); y++) {
    for (unsigned int x = 0; x < camera.getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 8, colourSampler));
    }
  }
  return image;
}

Colour Scene::sample(unsigned int x,
                     unsigned int y,
                     int nSamples,
                     const std::function<Colour(const Scene &, const Ray &)> &sampleFunction) {
  Colour pixelColour;
  double scale = 1.0 / nSamples;
  for (int i = 0; i < nSamples; i++) {
    double xOffset = random_double();
    double yOffset = random_double();
    Ray ray = camera.getRay(x, y, xOffset, yOffset);
    pixelColour += sampleFunction(*this, ray) * scale;
  }
  return pixelColour;
}

Colour Scene::sampleShaded(const Ray &ray, int depth) {
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  if (!intersection.has_value()) return backgroundColour;

  Vector3D normal = intersection.value().first->getSurfaceNormal(ray.at(intersection.value().second));

  // Taking absolute of negative values
  double r = std::fabs(normal.getX());
  double g = std::fabs(normal.getY());
  double b = std::fabs(normal.getZ());

  return Colour(r, g, b);
}

Colour Scene::sampleDiffuse(const Ray &ray, int depth) {
  // Base case: if we've reached the maximum number of bounces, return black
  if (depth >= nBounces) return Colour();

  // Check if the ray intersects with any objects in the scene
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  // If the ray doesn't intersect with anything, return the background backgroundColour
  if (!intersection.has_value()) return backgroundColour;

  // Get the normal of the surface at the intersection point
  Vector3D normal = intersection.value().first->getSurfaceNormal(ray.at(intersection.value().second));

  // Get a random direction in the hemisphere of the normal
  Vector3D bounceDirection = normal.randomInHemisphere();

  // Create a new ray starting at the intersection point and going in the bounce direction
  Ray bounceRay = Ray(ray.at(intersection.value().second), bounceDirection);

  // Get the diffuse factor and backgroundColour of the object
  double diffuseFactor = intersection.value().first->getMaterial().getKd();
  Colour diffuseColour = intersection.value().first->getMaterial().getDiffuseColour();

  // Calculate the backgroundColour of the bounce ray
  Colour bounceColour = sampleDiffuse(bounceRay, depth + 1);

  // Return the backgroundColour of the bounce ray multiplied by the diffuse factor and backgroundColour
  return Colour(diffuseColour * bounceColour * diffuseFactor);
}

std::optional<std::pair<std::shared_ptr<Shape>, double>> Scene::checkIntersection(const Ray &ray) const {
  std::shared_ptr<Shape> closestShape = nullptr;
  double closestT = 0;
  for (const auto &shape : shapes) {
    std::optional<double> t = shape->checkIntersection(ray);
    if (t.has_value() && (t.value() < closestT || closestShape == nullptr)) {
      closestShape = shape;
      closestT = t.value();
    }
  }
  if (closestShape == nullptr) {
    return std::nullopt;
  } else {
    return std::make_pair(closestShape, closestT);
  }
}
