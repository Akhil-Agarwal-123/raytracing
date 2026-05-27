#ifndef RAYTRACING_COLLISION_INFO_H
#define RAYTRACING_COLLISION_INFO_H

#include <memory>

#include "vec3.h"
#include "ray.h"
#include "../graphics/material.h"

class material;

class collision_info {
public:
    vec3 contact_point;
    vec3 normal;
    bool leaving;
    double distance;
    bool has_volume;
    std::shared_ptr<material> texture;

    void set_normal(const ray& ray, const vec3& outward_normal);
};

#endif //RAYTRACING_COLLISION_INFO_H
