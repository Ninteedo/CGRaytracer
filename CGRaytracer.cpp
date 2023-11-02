// CGRaytracer.cpp : Defines the entry point for the application.
//

#include "CGRaytracer.h"

#include "Image.h"

using namespace std;

int main()
{
	// cout << "Hello CMake." << endl;
	
	const unsigned int width = 256;
	const unsigned int height = 256;

    // Create an empty image with the specified dimensions
    Image img(width, height);

    // Fill the image with the gradient
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            float r = static_cast<float>(x) / width;
            float g = static_cast<float>(y) / height;
            float b = 0.0f; // Black for the blue channel

            // If we are in the bottom right quarter, make blue dominant
            if (x > width / 2 && y > height / 2) {
                b = 1.0f - r - g;
            }

            img.setColor(x, y, Image::Color(r, g, b));
        }
    }

    // Save the image to a PPM file
    try {
        img.saveToPPM("example.ppm");
        std::cout << "Successfully saved to example.ppm" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

	return 0;
}
