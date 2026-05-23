#ifndef RAYTRACING_IMAGE_H
#define RAYTRACING_IMAGE_H

#include "color.h"
#include <vector>

class output_image {
public:
    int height, width;
    std::vector<std::vector<color>> pixels;

    output_image(const int height, const int width) : height(height), width(width) {
        pixels = std::vector<std::vector<color>>(height);
        for (int i = 0; i < height; i++) {
            pixels[i] = std::vector<color>(width);
        }
    }

    void set_pixel(const int x, const int y, const color& c) {
        pixels[y][x] = c;
    }

    [[nodiscard]] color get_pixel(const int x, const int y) const {
        return pixels[y][x];
    }
};

inline void write_image(std::ostream& out, const output_image& image) {
    std::cout << "P3\n" << image.width << ' ' << image.height << "\n255\n";
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            write_color(out, image.pixels[i][j]);
        }
    }
}

#endif //RAYTRACING_IMAGE_H
