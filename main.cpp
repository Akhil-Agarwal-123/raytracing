#include <iostream>

#include "util/color.h"
#include "util/output_image.h"

int main() {
    output_image img(256, 256);
    for (int j = 0; j < img.height; j++) {
        for (int i = 0; i < img.width; i++) {
            color c(static_cast<double>(i) / (img.width-1), static_cast<double>(j) / (img.height-1), 0.0);
            img.set_pixel(i, j, c);
        }
    }
    write_image(std::cout, img);
}
