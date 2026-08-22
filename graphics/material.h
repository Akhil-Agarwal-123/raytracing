#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "color.h"
#include "../geometry/vec3.h"
#include "../util/rng.h"

class collision_info;
class hittable_list;
class ray;

class material {
public:
    float refractive_index = 1.0f;
    color absorption_rates;

    virtual ~material() = default;
    virtual bool get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                              color &albedo, vec3 &next_dir, bool &reflecting,
                              rng::Sampler &sampler) const = 0;

protected:
    static vec3 random_in_hemisphere(const vec3 &normal, rng::Sampler &sampler);
    static vec3 reflected(const ray &incoming, const vec3 &normal);
};

class lambertian : public material {
public:
    explicit lambertian(color c);
    bool get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                      color &albedo, vec3 &next_dir, bool &reflecting,
                      rng::Sampler &sampler) const override;

private:
    color c;
};

class metal : public material {
public:
    explicit metal(color c);
    bool get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                      color &albedo, vec3 &next_dir, bool &reflecting,
                      rng::Sampler &sampler) const override;

private:
    color c;
};

class dielectric : public material {
public:
    dielectric(const color& absorption_rates, float object_reflectivity, float refractive_index);
    bool get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                      color &albedo, vec3 &next_dir, bool &reflecting,
                      rng::Sampler &sampler) const override;

private:
    float object_reflectivity;

    float get_fresnel_reflection_amount(const ray &incoming, const collision_info &hit_info) const;
};

class light_source : public material {
public:
    explicit light_source(const color& c);
    bool get_next_ray(const ray &incoming_ray, const collision_info &hit_info,
                      color &albedo, vec3 &next_dir, bool &reflecting,
                      rng::Sampler &sampler) const override;

private:
    color c;
};

#endif // RAYTRACING_TEXTURE_H
