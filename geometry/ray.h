#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "vec3.h"

class ray {
public:
    ray(const vec3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    [[nodiscard]] vec3 origin() const { return orig; }
    [[nodiscard]] vec3 direction() const { return dir; }

    [[nodiscard]] vec3 at(const double t) const {
        return orig + t * dir;
    }

private:
    vec3 orig, dir;
};

#endif //RAYTRACING_RAY_H
