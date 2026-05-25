#include "material.h"

#include "color.h"
#include "../geometry/ray.h"
#include "../geometry/collision_info.h"
#include "../objects/hittable_list.h"
#include "../util/rng.h"

vec3 material::random_in_hemisphere(const vec3& normal) {
    vec3 r = rng::random_unit_vector();
    double dot_prod = dot(r, normal);
    if (dot_prod > 0.0) return r;
    if (dot_prod == 0.0) return normal;
    return -r;
}

vec3 material::reflected(const ray& incoming, const vec3& normal) {
    return incoming.direction() - 2 * dot(incoming.direction(), normal) * normal;
}

lambertian::lambertian(color c) : c(c) {}

color lambertian::get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    auto dir = unit_vector(hit_info.normal + rng::random_unit_vector());
    auto next_ray = ray(hit_info.contact_point, dir, incoming_ray.get_mat_stack());
    if (dir.near_zero()) next_ray = ray(hit_info.contact_point, hit_info.normal, incoming_ray.get_mat_stack());
    return c * scene.get_raytraced_color(next_ray, max_bounces);
}

metal::metal(color c) : c(c) {}

color metal::get_color(const hittable_list& scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    auto next_ray = ray(hit_info.contact_point, reflected(incoming_ray, hit_info.normal), incoming_ray.get_mat_stack());
    return c * scene.get_raytraced_color(next_ray, max_bounces);
}

dielectric::dielectric(color absorption_rates, double object_reflectivity, double refractive_index)
    : absorption_rates(absorption_rates), object_reflectivity(object_reflectivity) {
    this->refractive_index = refractive_index;
}

color dielectric::get_color(const hittable_list& scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    double reflection_fraction = get_fresnel_reflection_amount(incoming_ray, hit_info);
    if (rng::random_double() <= reflection_fraction) {
        // reflect
        ray next_ray = ray(hit_info.contact_point, reflected(incoming_ray, hit_info.normal), incoming_ray.get_mat_stack());
        return scene.get_raytraced_color(next_ray, max_bounces) * exp(-absorption_rates * hit_info.distance * hit_info.leaving);
    } else {
        // refract
        double n1 = hit_info.leaving ? refractive_index : incoming_ray.n();
        double n2 = 1.0;

        if (hit_info.leaving) {
            auto current_stack = incoming_ray.get_mat_stack(); // Fetch ONE copy
            current_stack.pop();
            if (!current_stack.empty()) n2 = current_stack.top()->refractive_index;
        } else {
            n2 = refractive_index;
        }

        vec3 next_direction_perp = n1/n2 * (incoming_ray.direction() - dot(incoming_ray.direction(), hit_info.normal) * hit_info.normal);
        vec3 next_direction_par = -hit_info.normal * sqrt(1 - next_direction_perp.length_squared());
        vec3 next_dir = next_direction_par + next_direction_perp;
        ray next_ray = ray(hit_info.contact_point, next_dir, incoming_ray.get_mat_stack());
        if (hit_info.leaving) next_ray.remove_last_mat();
        else next_ray.add_new_mat(hit_info.texture);
        auto ret = scene.get_raytraced_color(next_ray, max_bounces) * exp(-absorption_rates * hit_info.distance * hit_info.leaving);
        return ret;
    }
}

double dielectric::get_fresnel_reflection_amount(const ray& incoming, const collision_info& hit_info) const {
    double n1 = hit_info.leaving ? refractive_index : incoming.n();
    double n2 = 1.0;

    if (hit_info.leaving) {
        auto current_stack = incoming.get_mat_stack();
        if (!current_stack.empty()) current_stack.pop();
        if (!current_stack.empty()) n2 = current_stack.top()->refractive_index;
    } else {
        n2 = refractive_index;
    }

    double r0 = (n1 - n2)/(n1 + n2);
    r0 *= r0;
    double cosX = -dot(hit_info.normal, incoming.direction());
    if (n1 > n2) {
        double n = n1/n2;
        double sinT2 = n*n*(1.0 - cosX*cosX);
        if (sinT2 > 1.0) return 1.0;
        cosX = sqrt(1.0 - sinT2);
    }
    double x = 1.0 - cosX;
    double ret = r0 + (1.0 - r0) * x*x*x*x*x;
    ret = object_reflectivity + (1.0 - object_reflectivity) * ret;
    return ret;
}

color light_source::get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) {
    return {1.0, 1.0, 1.0};
}