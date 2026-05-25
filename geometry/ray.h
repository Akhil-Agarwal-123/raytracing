#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "vec3.h"

class hittable;

class ray {
public:
    ray(const vec3& origin, const vec3& direction, const double refractive_index) : orig(origin), dir(direction), refractive_index(refractive_index) {}

    vec3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    double n() const { return refractive_index; }

    vec3 at(const double t) const {
        return orig + t * dir;
    }

private:
    vec3 orig, dir;
    double refractive_index;
};

#endif //RAYTRACING_RAY_H
