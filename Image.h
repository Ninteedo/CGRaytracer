#ifndef IMAGE_H
#define IMAGE_H

#include <string>

class Image {
public:
    struct Color {
        float red;
        float green;
        float blue;
        float alpha;

        Color(float r = 0, float g = 0, float b = 0, float a = 1);
    };

    Image(unsigned int width, unsigned int height);
    ~Image();

    Color getColor(unsigned int x, unsigned int y) const;
    void setColor(unsigned int x, unsigned int y, const Color& color);

    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void saveToPPM(const std::string& filename) const;

private:
    unsigned int width;
    unsigned int height;
    Color* pixels;

    bool isInside(unsigned int x, unsigned int y) const;
};

#endif // IMAGE_H
