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
