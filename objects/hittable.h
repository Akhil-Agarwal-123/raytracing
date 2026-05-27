#ifndef RAYTRACING_HITTABLE_H
#define RAYTRACING_HITTABLE_H

#include <memory>

#include "../geometry/aabb.h"
#include "../geometry/ray.h"

class material;
class collision_info;

class hittable {
public:
    std::shared_ptr<material> mat;

    virtual ~hittable() = default;
    virtual bool hit(ray &r, collision_info &hit_info) const = 0;

    virtual aabb get_bounding_box() const = 0;
    virtual vec3 centroid() const = 0;
};

#endif //RAYTRACING_HITTABLE_H
