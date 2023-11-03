#include "Scene.h"

#include <utility>

Scene::Scene(Colour colour,
             const Camera &camera,
             std::vector<std::shared_ptr<Shape>> objects,
             std::vector<std::shared_ptr<LightSource>> lightSources,
             int nBounces,
             RenderMode renderMode)
    : colour(std::move(colour)),
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
  colour = Colour(sceneObject["backgroundcolor"].asArray());
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
    return renderBinary();
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
        pixelColour = colour;
      }

      image.setColor(x, y, pixelColour);
    }
  }
  return image;
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
