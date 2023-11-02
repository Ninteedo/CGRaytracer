#ifndef CGRAYTRACER_SCENE_H
#define CGRAYTRACER_SCENE_H

#include "JSONParser.h"
#include "Colour.h"
#include "Object.h"
#include "LightSource.h"

class Scene {
private:
    Colour colour;
    std::vector<Object> objects;
    std::vector<LightSource> lightSources;
public:
    Scene(Colour colour, std::vector<Object> objects, std::vector<LightSource> lightSources);
    Scene(JsonObject sceneJson);
    ~Scene();
};

#endif //CGRAYTRACER_SCENE_H
