#ifndef RAYTRACING_COLLISION_INFO_H
#define RAYTRACING_COLLISION_INFO_H

#include <memory>

#include "vec3.h"
#include "../graphics/material.h"

class material;

class collision_info {
public:
    vec3 contact_point;
    vec3 normal;
    vec3 geometric_normal;
    bool leaving;
    float distance;
    bool has_volume;
    std::shared_ptr<material> texture;

    void set_normal(const ray& ray, const vec3& outward_normal);
    void set_normal(const ray& ray, const vec3& outward_shading_normal, const vec3& geo_normal);
};

#endif //RAYTRACING_COLLISION_INFO_H
