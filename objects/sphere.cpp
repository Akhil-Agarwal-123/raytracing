#include "sphere.h"

#include "hittable.h"
#include "../geometry/collision_info.h"
#include "../geometry/vec3.h"

sphere::sphere(const double radius, const vec3 &center, const std::shared_ptr<material> &t) : radius(radius), center(center) {
    mat = t;
}

bool sphere::hit(ray& r, collision_info& hit_info) const {
    vec3 dist = center - r.orig;
    double b = -2 * dot(dist, r.dir);
    double c = dot(dist, dist) - radius * radius;
    double discriminant = b * b - 4 * c;
    if (discriminant <= 0) {
        return false;
    }
    double sqrt_discriminant = sqrt(discriminant);
    double t = (-b - sqrt_discriminant)/2;
    if (t <= 0.0) {
        t = (-b + sqrt_discriminant)/2;
        if (t <= 0.0) {
            return false;
        }
    }
    hit_info.contact_point = r.at(t);
    hit_info.set_normal(r, (hit_info.contact_point - center)/radius);
    hit_info.distance = t;
    hit_info.texture = mat;
    hit_info.has_volume = true;
    return true;
}

aabb sphere::get_bounding_box() const {
    return {center - radius * vec3(1, 1, 1), center + radius * vec3(1, 1, 1)};
}

vec3 sphere::centroid() const {
    return center;
}
