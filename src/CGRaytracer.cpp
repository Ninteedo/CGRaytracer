// CGRaytracer.cpp : Defines the entry point for the application.
//

#include "CGRaytracer.h"

#include "Image.h"
#include "Scene.h"

using namespace std;

int main(int argc, char *argv[]) {
  // Check if a scene filename is passed as an argument
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <sceneFilename>" << endl;
    return 1;
  }

  string sceneFilename = argv[1]; // Get the scene filename from the command line

  Scene scene = Scene::loadFromFile("../scenes/" + sceneFilename);

  Image rendered = scene.render();
  rendered.saveToPPM("../output/" + sceneFilename + ".ppm");
}
