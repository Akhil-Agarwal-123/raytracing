#ifndef RAYTRACING_COLOR_H
#define RAYTRACING_COLOR_H

#include "../geometry/vec3.h"
#include <iostream>
#include <cmath>

using color = vec3;

#if defined(__CUDACC__)
#define CUDA_HOST_DEVICE __host__ __device__
#else
#define CUDA_HOST_DEVICE
#endif

CUDA_HOST_DEVICE inline float linear_to_gamma(float linear_component) {
    if (linear_component > 0.0f) {
        return sqrtf(linear_component);
    }
    return 0.0f;
}

#ifndef __CUDA_ARCH__
inline void write_color(std::ostream& out, const color& pixel_color) {
    // Apply gamma correction (gamma 2.0) with NaN and overflow protection
    float r = linear_to_gamma(pixel_color.x());
    float g = linear_to_gamma(pixel_color.y());
    float b = linear_to_gamma(pixel_color.z());

    // Clamp to [0.0, 0.999] before integer scaling
    r = fminf(0.999f, fmaxf(0.0f, r));
    g = fminf(0.999f, fmaxf(0.0f, g));
    b = fminf(0.999f, fmaxf(0.0f, b));

    int rbyte = static_cast<int>(256.0f * r);
    int gbyte = static_cast<int>(256.0f * g);
    int bbyte = static_cast<int>(256.0f * b);

    // Write out the pixel color components
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
#endif

#endif // RAYTRACING_COLOR_H