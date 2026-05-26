#include "bvh.h"

bvh::bvh(const aabb& bbox, const std::vector<int>& hittable_indices) : bounding_box(bbox), hittable_indices(hittable_indices) {
    left_child = nullptr;
    right_child = nullptr;
    leaf_node = true;
}

bvh::bvh(const aabb& bbox, const std::shared_ptr<bvh>& left_child, const std::shared_ptr<bvh>& right_child)
    : bounding_box(bbox), left_child(left_child), right_child(right_child) {
    leaf_node = false;
}

hittable_info::hittable_info(const int idx, const aabb& bbox, const vec3& centroid) : idx(idx), bbox(bbox), centroid(centroid) {}

std::shared_ptr<bvh> make_bvh(const std::vector<std::shared_ptr<hittable>> &scene) {
    std::vector<hittable_info> hittables;
    hittables.reserve(scene.size());
    for (int i = 0; i < scene.size(); ++i) {
        hittables.emplace_back(i, scene[i]->get_bounding_box(), scene[i]->centroid());
    }
    return make_bvh(hittables, 0, static_cast<int>(hittables.size()));
}

std::shared_ptr<bvh> make_bvh(std::vector<hittable_info> &scene, int st, int en) {
    // todo: implement
}

bool hit_bvh(const std::shared_ptr<bvh> &root, ray &r, collision_info &hit_info) {
    // todo: implement
}
