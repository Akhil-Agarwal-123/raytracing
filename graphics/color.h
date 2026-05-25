#ifndef RAYTRACING_COLOR_H
#define RAYTRACING_COLOR_H

#include "../geometry/vec3.h"

#include <iostream>

using color = vec3;

inline void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Translate the [0,1] component values to the byte range [0,255].
    int rbyte = static_cast<int>(255.999 * r);
    int gbyte = static_cast<int>(255.999 * g);
    int bbyte = static_cast<int>(255.999 * b);

    rbyte = std::max(rbyte, 0);
    gbyte = std::max(gbyte, 0);
    bbyte = std::max(bbyte, 0);

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif //RAYTRACING_COLOR_H
