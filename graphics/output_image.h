#ifndef RAYTRACING_IMAGE_H
#define RAYTRACING_IMAGE_H

#include <vector>
#include <ostream>
#include "color.h"

class output_image {
public:
    int height, width;
    std::vector<color> pixels;

    output_image(const int height, const int width)
        : height(height), width(width), pixels(width * height) {}

    inline void set_pixel(const int x, const int y, const color& c) {
        pixels[y * width + x] = c;
    }

    [[nodiscard]] inline color get_pixel(const int x, const int y) const {
        return pixels[y * width + x];
    }
};

inline void write_image(std::ostream& out, const output_image& image) {
    out << "P3\n" << image.width << ' ' << image.height << "\n255\n";
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            write_color(out, image.pixels[i * image.width + j]);
        }
    }
}

#endif // RAYTRACING_IMAGE_H