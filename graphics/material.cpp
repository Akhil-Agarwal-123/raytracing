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

vec3 material::reflected(const ray &incoming, const vec3& normal) {
    return incoming.dir - 2 * dot(incoming.dir, normal) * normal;
}

lambertian::lambertian(color c) : c(c) {}

bool lambertian::get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) {
    next_dir = unit_vector(hit_info.normal + rng::random_unit_vector());
    if (next_dir.near_zero()) next_dir = hit_info.normal;
    albedo = c;
    reflecting = true;
    return true;
}

metal::metal(color c) : c(c) {}

bool metal::get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) {
    albedo = c;
    next_dir = reflected(incoming_ray, hit_info.normal);
    reflecting = true;
    return true;
}

dielectric::dielectric(const color& absorption_rates, const double object_reflectivity, const double refractive_index)
    : object_reflectivity(object_reflectivity) {
    this->refractive_index = refractive_index;
    this->absorption_rates = absorption_rates;
}

bool dielectric::get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) {
    double reflection_fraction = get_fresnel_reflection_amount(incoming_ray, hit_info);
    if (rng::random_double() <= reflection_fraction) {
        reflecting = true;
        next_dir = reflected(incoming_ray, hit_info.normal);
    } else {
        reflecting = false;

        double n1 = hit_info.leaving ? refractive_index : incoming_ray.n();
        double n2 = hit_info.leaving ? incoming_ray.next_n() : refractive_index;

        vec3 next_direction_perp = n1/n2 * (incoming_ray.dir - dot(incoming_ray.dir, hit_info.normal) * hit_info.normal);
        vec3 next_direction_par = -hit_info.normal * sqrt(1 - next_direction_perp.length_squared());
        next_dir = next_direction_par + next_direction_perp;
    }
    albedo = exp(-absorption_rates * hit_info.distance * hit_info.leaving);
    return true;
}

double dielectric::get_fresnel_reflection_amount(ray &incoming, const collision_info &hit_info) const {
    double n1 = hit_info.leaving ? refractive_index : incoming.n();
    double n2 = hit_info.leaving ? incoming.next_n() : refractive_index;

    double r0 = (n1 - n2)/(n1 + n2);
    r0 *= r0;
    double cosX = -dot(hit_info.normal, incoming.dir);
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

light_source::light_source(const color& c) : c(c) {}

bool light_source::get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) {
    albedo = c;
    return false;
}