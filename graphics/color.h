#ifndef RAYTRACING_COLOR_H
#define RAYTRACING_COLOR_H

#include "../geometry/vec3.h"

#include <iostream>

using color = vec3;

inline double linear_to_gamma(double linear_component) {
    return std::sqrt(linear_component);
}

inline void write_color(std::ostream& out, const color& pixel_color) {
    auto r = linear_to_gamma(pixel_color.x());
    auto g = linear_to_gamma(pixel_color.y());
    auto b = linear_to_gamma(pixel_color.z());

    int rbyte = static_cast<int>(255.999 * r);
    int gbyte = static_cast<int>(255.999 * g);
    int bbyte = static_cast<int>(255.999 * b);

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif //RAYTRACING_COLOR_H
