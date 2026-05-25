#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "color.h"

class collision_info;
class hittable_list;
class ray;

class material {
public:
    double refractive_index = 1.0;
    color absorption_rates;

    virtual ~material() = default;
    virtual bool get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) = 0;

protected:
    static vec3 random_in_hemisphere(const vec3 &normal);
    static vec3 reflected(const ray &incoming, const vec3 &normal);
};

class lambertian : public material {
public:
    lambertian(color c);
    bool get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) override;

private:
    color c;
};

class metal : public material {
public:
    metal(color c);
    bool get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) override;

private:
    color c;
};

class dielectric : public material {
public:
    dielectric(const color& absorption_rates, double object_reflectivity, double refractive_index);
    bool get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) override;

private:
    double object_reflectivity;

    double get_fresnel_reflection_amount(ray &incoming, const collision_info &hit_info) const;
};

class light_source : public material {
public:
    light_source(const color& c);
    bool get_next_ray(ray &incoming_ray, const collision_info &hit_info, color &albedo, vec3 &next_dir, bool &reflecting) override;
    
private:
    color c;
};

#endif //RAYTRACING_TEXTURE_H
