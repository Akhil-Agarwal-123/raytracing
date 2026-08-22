#include "sphere.h"

#include <cmath>
#include "hittable.h"
#include "../geometry/collision_info.h"
#include "../geometry/vec3.h"

sphere::sphere(const float radius, const vec3 &center, const std::shared_ptr<material> &t)
    : radius(radius), center(center) {
    mat = t;
}

bool sphere::hit(ray& r, collision_info& hit_info) const {
    vec3 dist = center - r.orig;
    float h = dot(dist, r.dir);
    float c = dot(dist, dist) - radius * radius;
    float discriminant = h * h - c;

    if (discriminant < 0.0f) {
        return false;
    }

    float sqrt_discriminant = sqrtf(discriminant);

    // Find the nearest root that lies within the acceptable range [0.001, infinity)
    float t = h - sqrt_discriminant;
    if (t < 0.001f) {
        t = h + sqrt_discriminant;
        if (t < 0.001f) {
            return false;
        }
    }

    hit_info.distance = t;
    hit_info.contact_point = r.at(t);

    float inv_radius = 1.0f / radius;
    vec3 outward_normal = (hit_info.contact_point - center) * inv_radius;
    hit_info.set_normal(r, outward_normal);

    hit_info.texture = mat;
    hit_info.has_volume = true;
    return true;
}

aabb sphere::get_bounding_box() const {
    vec3 r_vec(radius, radius, radius);
    return {center - r_vec, center + r_vec};
}

vec3 sphere::centroid() const {
    return center;
}