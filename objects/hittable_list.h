#ifndef RAYTRACING_HITTABLE_LIST_H
#define RAYTRACING_HITTABLE_LIST_H

#include <memory>
#include <vector>
#include "hittable.h"
#include "../geometry/ray.h"
#include "../graphics/color.h"
#include "../geometry/collision_info.h"
#include "../graphics/material.h" // Safe to include!

class hittable_list {
public:
    void add_hittable(const std::shared_ptr<hittable>& hittable);

    color get_raytraced_color(ray r, int max_bounces = 10) const;

private:
    std::vector<std::shared_ptr<hittable>> hittables;
};

#endif //RAYTRACING_HITTABLE_LIST_H