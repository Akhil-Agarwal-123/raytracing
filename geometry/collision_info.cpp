#include "collision_info.h"
#include "vec3.h"
#include "ray.h"

void collision_info::set_normal(const ray& ray, const vec3& outward_normal) {
    if (dot(ray.dir, outward_normal) > 0.0) {
        normal = -outward_normal;
        leaving = true;
    } else {
        normal = outward_normal;
        leaving = false;
    }
    geometric_normal = normal;
}

void collision_info::set_normal(const ray& ray, const vec3& outward_shading_normal, const vec3& geo_normal) {
    if (dot(ray.dir, outward_shading_normal) > 0.0) {
        normal = -outward_shading_normal;
        geometric_normal = -geo_normal;
        leaving = true;
    } else {
        normal = outward_shading_normal;
        geometric_normal = geo_normal;
        leaving = false;
    }
}
