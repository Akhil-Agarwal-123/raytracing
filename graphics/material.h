#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "color.h"
#include "../geometry/ray.h"

class collision_info;
class hittable_list;

class material {
public:
    double refractive_index;

    virtual ~material() = default;
    virtual color get_color(const hittable_list &scene, const ray &incoming_ray, const collision_info &hit_info, int max_bounces) = 0;

protected:
    static vec3 random_in_hemisphere(const vec3 &normal);
    static vec3 reflected(const ray &incoming, const vec3 &normal);
};

class lambertian : public material {
public:
    lambertian(color c);
    color get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) override;

private:
    color c;
};

class metal : public material {
public:
    metal(color c);
    color get_color(const hittable_list& scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) override;

private:
    color c;
};

class dielectric : public material {
public:
    dielectric(color absorption_rates, double object_reflectivity, double refractive_index);
    color get_color(const hittable_list& scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) override;

private:
    color absorption_rates;
    double object_reflectivity;

    double get_fresnel_reflection_amount(const ray &incoming, const collision_info& hit_info) const;
};

class light_source : public material {
public:
    color get_color(const hittable_list &scene, const ray& incoming_ray, const collision_info& hit_info, int max_bounces) override;
};

#endif //RAYTRACING_TEXTURE_H
