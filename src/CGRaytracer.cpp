// CGRaytracer.cpp : Defines the entry point for the application.
//

#include "CGRaytracer.h"

#include "Image.h"
#include "Scene.h"

using namespace std;

int main() {
  std::string sceneFilename = "mirror_image.json";

  Scene scene = Scene::loadFromFile("../scenes/" + sceneFilename);

  Image rendered = scene.render();
  rendered.saveToPPM("../output/" + sceneFilename + ".ppm");
}
