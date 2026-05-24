#include "material.h"

#include "color.h"
#include "../geometry/ray.h"
#include "../geometry/collision_info.h"
#include "../objects/hittable_list.h"
#include "../util/rng.h"

matte::matte(color c) : c(c) {}

color matte::get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    ray next_ray = ray(hit_info.contact_point, random_in_hemisphere(hit_info.normal));
    return c * scene.get_raytraced_color(next_ray, max_bounces);
}

vec3 matte::random_in_hemisphere(const vec3& normal) {
    while (true) {
        vec3 r = rng::random_unit_vector();
        if (dot(r, normal) > 0.0) return r;
    }
}

lambertian::lambertian(color c) : c(c) {}

color lambertian::get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    auto dir = hit_info.normal + rng::random_unit_vector();
    ray next_ray = ray(hit_info.contact_point, dir);
    if (dir.near_zero()) next_ray = ray(hit_info.contact_point, hit_info.normal);
    return c * scene.get_raytraced_color(next_ray, max_bounces);
}

metal::metal(color c) : c(c) {}

color metal::get_color(const hittable_list& scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    ray next_ray = ray(hit_info.contact_point, reflected(incoming_ray, hit_info.normal));
    return c * scene.get_raytraced_color(next_ray, max_bounces);
}

vec3 metal::reflected(const ray& incoming, const vec3& normal) {
    return incoming.direction() - 2 * dot(incoming.direction(), normal) * normal;
}

color light_source::get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    return {1.0, 1.0, 1.0};
}