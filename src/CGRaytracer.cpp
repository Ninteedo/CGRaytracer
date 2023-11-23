// CGRaytracer.cpp : Defines the entry point for the application.
//

#include "CGRaytracer.h"

#include "Image.h"
#include "Scene.h"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  // Check if the command line option for batch processing is passed
  if (argc >= 2 && string(argv[1]) == "--batch-process") {
    string inputDir = R"(..\scenes\animation\)";
    string outputDir = R"(..\output\animation\)";

    // Check and create output directory if it doesn't exist
    if (!fs::exists(outputDir) && !fs::create_directories(outputDir)) {
      cerr << "Failed to create output directory." << endl;
      return 1;
    }

    // Iterate over scene files in the input directory
    for (const auto& entry : fs::directory_iterator(inputDir)) {
      string sceneFilename = entry.path().string();
      string outputFilename = outputDir + entry.path().filename().replace_extension(".ppm").string();

      Scene scene = Scene::loadFromFile(sceneFilename);

      Image rendered = scene.render();
      rendered.saveToPPM(outputFilename);

      cout << "Rendered " << sceneFilename << " to " << outputFilename << endl;
    }
  } else if (argc == 3) {
    // Single file processing
    string sceneFilename = argv[1];
    string outputFilename = argv[2];

    Scene scene = Scene::loadFromFile(sceneFilename);

    Image rendered = scene.render();
    rendered.saveToPPM(outputFilename);
  } else {
    cerr << "Usage: " << argv[0] << " <sceneFilename> <outputFilename>" << endl;
    cerr << "   or: " << argv[0] << " --batch-process" << endl;
    return 1;
  }

  return 0;
}
