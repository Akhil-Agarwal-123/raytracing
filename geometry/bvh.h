#ifndef RAYTRACING_BVH_H
#define RAYTRACING_BVH_H

#include "aabb.h"
#include <vector>
#include <memory>

#include "../objects/hittable.h"

class bvh {
public:
    aabb bounding_box;
    std::shared_ptr<bvh> left_child, right_child;
    bool leaf_node;
    std::vector<int> hittable_indices;

    bvh(const aabb& bbox, const std::vector<int>& hittable_indices);
    bvh(const aabb& bbox, const std::shared_ptr<bvh>& left_child, const std::shared_ptr<bvh>& right_child);
};

class hittable_info {
public:
    int idx;
    aabb bbox;
    vec3 centroid;

    hittable_info(int idx, const aabb& bbox, const vec3& centroid);
};

std::shared_ptr<bvh> make_bvh(const std::vector<std::shared_ptr<hittable>> &scene);
std::shared_ptr<bvh> make_bvh(std::vector<hittable_info> &scene, int st, int en);

bool hit_bvh(const std::vector<std::shared_ptr<hittable>> &scene, const std::shared_ptr<bvh> &root, ray &r, collision_info &hit_info);

#endif //RAYTRACING_BVH_H
