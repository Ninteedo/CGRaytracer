#include "Scene.h"

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
  for (int i = 0; i < lightsJson.size(); i++) {
    lightSources.push_back(LightSource::fromJson(lightsJson[i].asObject()));
  }
  JsonArray objectsJson = sceneObject["shapes"].asArray();
  for (int i = 0; i < objectsJson.size(); i++) {
    objects.push_back(Shape::fromJson(objectsJson[i].asObject()));
  }
}

Scene Scene::loadFromFile(const std::string& filename) {
    JsonParser parser;
    return {parser.parseFile(filename).asObject()};
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
      Intersection intersection = checkIntersection(ray, 1);

      Colour pixelColour;
      if (intersection.hit) {
        pixelColour = Colour(1, 0, 0);
      } else {
        pixelColour = colour;
      }

      image.setColor(x, y, pixelColour);
    }
  }
  return image;
}
