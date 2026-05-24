#ifndef RAYTRACING_HITTABLE_H
#define RAYTRACING_HITTABLE_H

#include <memory>
#include "../geometry/ray.h"

class material;
class collision_info;

class hittable {
public:
    std::shared_ptr<material> mat;
    virtual ~hittable() = default;
    virtual bool hit(ray &r, collision_info &hit_info) = 0;

protected:
    double eps = 1e-8;
};

#endif //RAYTRACING_HITTABLE_H
