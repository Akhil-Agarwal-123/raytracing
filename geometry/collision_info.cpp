#include "collision_info.h"
#include "vec3.h"
#include "ray.h"

void collision_info::set_normal(const ray& ray, const vec3& outward_normal) {
    if (dot(ray.direction(), outward_normal) > 0.0) {
        normal = -outward_normal;
        leaving = true;
    } else {
        normal = outward_normal;
        leaving = false;
    }
}
