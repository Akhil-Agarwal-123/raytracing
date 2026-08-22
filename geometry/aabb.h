#ifndef RAYTRACING_AABB_H
#define RAYTRACING_AABB_H

#include "ray.h"
#include "vec3.h"

class aabb {
public:
    bool trivial;
    vec3 low, high;

    aabb();
    aabb(const vec3& low, const vec3& high);

    bool hit(const ray &incoming_ray, float &distance) const;
    void combine(const aabb &other);
    void combine(const vec3 &other);
    [[nodiscard]] float surface_area() const;
    [[nodiscard]] int longest_axis() const;
};

#endif // RAYTRACING_AABB_H