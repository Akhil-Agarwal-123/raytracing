#include "material.h"

#include <cmath>
#include <algorithm>
#include "color.h"
#include "../geometry/ray.h"
#include "../geometry/collision_info.h"
#include "../util/rng.h"

vec3 material::random_in_hemisphere(const vec3& normal, rng::Sampler& sampler) {
    vec3 r = rng::random_unit_vector(sampler);
    float dot_prod = dot(r, normal);
    if (dot_prod > 0.0f) return r;
    if (dot_prod == 0.0f) return normal;
    return -r;
}

vec3 material::reflected(const ray &incoming, const vec3& normal) {
    return incoming.dir - 2.0f * dot(incoming.dir, normal) * normal;
}

lambertian::lambertian(color c) : c(c) {}

bool lambertian::get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                              color &albedo, vec3 &next_dir, bool &reflecting,
                              rng::Sampler &sampler) const {
    next_dir = unit_vector(hit_info.normal + rng::random_unit_vector(sampler));
    if (next_dir.near_zero()) next_dir = hit_info.normal;
    albedo = c;
    reflecting = true;
    return true;
}

metal::metal(color c) : c(c) {}

bool metal::get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                        color &albedo, vec3 &next_dir, bool &reflecting,
                        rng::Sampler &sampler) const {
    albedo = c;
    next_dir = reflected(incoming_ray, hit_info.normal);
    reflecting = true;
    return true;
}

dielectric::dielectric(const color& absorption_rates, const float object_reflectivity, const float refractive_index)
    : object_reflectivity(object_reflectivity) {
    this->refractive_index = refractive_index;
    this->absorption_rates = absorption_rates;
}

bool dielectric::get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                              color &albedo, vec3 &next_dir, bool &reflecting,
                              rng::Sampler &sampler) const {
    float reflection_fraction = get_fresnel_reflection_amount(incoming_ray, hit_info);

    if (rng::random_float(sampler) <= reflection_fraction) {
        reflecting = true;
        next_dir = reflected(incoming_ray, hit_info.normal);
    } else {
        reflecting = false;

        float n1 = hit_info.leaving ? refractive_index : incoming_ray.n();
        float n2 = hit_info.leaving ? incoming_ray.next_n() : refractive_index;
        float eta = n1 / n2;

        vec3 next_direction_perp = eta * (incoming_ray.dir - dot(incoming_ray.dir, hit_info.normal) * hit_info.normal);
        float perp_len_sq = next_direction_perp.length_squared();

        // Guard against precision errors producing negative values under sqrtf
        vec3 next_direction_par = -hit_info.normal * sqrtf(fmaxf(0.0f, 1.0f - perp_len_sq));
        next_dir = next_direction_par + next_direction_perp;
    }

    // Beer's law absorption only applies when ray travels through the internal volume (leaving == true)
    float dist = hit_info.leaving ? hit_info.distance : 0.0f;
    albedo = exp(-absorption_rates * dist);
    return true;
}

float dielectric::get_fresnel_reflection_amount(const ray &incoming, const collision_info &hit_info) const {
    float n1 = hit_info.leaving ? refractive_index : incoming.n();
    float n2 = hit_info.leaving ? incoming.next_n() : refractive_index;

    float r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;

    float cosX = fminf(1.0f, -dot(hit_info.normal, incoming.dir));

    if (n1 > n2) {
        float n = n1 / n2;
        float sinT2 = n * n * (1.0f - cosX * cosX);
        if (sinT2 > 1.0f) return 1.0f; // Total Internal Reflection
        cosX = sqrtf(fmaxf(0.0f, 1.0f - sinT2));
    }

    float x = 1.0f - cosX;
    float x2 = x * x;
    float x5 = x2 * x2 * x;

    float ret = r0 + (1.0f - r0) * x5;
    ret = object_reflectivity + (1.0f - object_reflectivity) * ret;
    return ret;
}

light_source::light_source(const color& c) : c(c) {}

bool light_source::get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                               color &albedo, vec3 &next_dir, bool &reflecting,
                               rng::Sampler &sampler) const {
    albedo = c;
    return false;
}