#include "collision_info.h"
#include "vec3.h"
#include "ray.h"

void collision_info::set_normal(const ray& ray, const vec3& outward_normal) {
    if (dot(ray.dir, outward_normal) > 0.0f) {
        normal = -outward_normal;
        leaving = true;
    } else {
        normal = outward_normal;
        leaving = false;
    }
    geometric_normal = normal;
}

void collision_info::set_normal(const ray& ray, const vec3& outward_shading_normal, const vec3& geo_normal) {
    // True surface geometry determines physical entering vs leaving
    bool is_leaving = dot(ray.dir, geo_normal) > 0.0f;
    leaving = is_leaving;

    geometric_normal = is_leaving ? -geo_normal : geo_normal;
    
    // Flip shading normal to match the side the ray hit
    normal = (dot(ray.dir, outward_shading_normal) > 0.0f) ? -outward_shading_normal : outward_shading_normal;
}