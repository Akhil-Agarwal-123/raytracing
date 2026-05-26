#ifndef RAYTRACING_AABB_H
#define RAYTRACING_AABB_H

#include "ray.h"
#include "vec3.h"

class aabb {
public:
    vec3 low, high;

    aabb(vec3 low, vec3 high);
    bool hit(const ray &incoming_ray, double &distance) const;
    void combine(const aabb &other);
    double surface_area() const;
};


#endif //RAYTRACING_AABB_H
