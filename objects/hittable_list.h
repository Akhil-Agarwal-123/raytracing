#ifndef RAYTRACING_HITTABLE_LIST_H
#define RAYTRACING_HITTABLE_LIST_H

#include <memory>
#include <vector>
#include "hittable.h"
#include "../geometry/bvh.h"
#include "../geometry/ray.h"
#include "../graphics/color.h"
#include "../util/rng.h"

class hittable_list {
public:
    std::shared_ptr<bvh> bounding_volume_hierarchy;
    std::vector<std::shared_ptr<hittable>> hittables;

    void add_hittable(const std::shared_ptr<hittable>& hittable);
    void mark_done();

    color get_raytraced_color(ray r, int max_bounces, rng::Sampler& sampler) const;
};

#endif // RAYTRACING_HITTABLE_LIST_H