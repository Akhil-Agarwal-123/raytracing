#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include "hittable.h"
#include "../geometry/vec3.h"

class sphere : public hittable {
public:
    sphere(const double radius, const vec3 &center, const std::shared_ptr<material> &t);
    bool hit(ray& r, collision_info& hit_info) override;

private:
    double radius;
    vec3 center;
};

#endif //RAYTRACING_SPHERE_H
