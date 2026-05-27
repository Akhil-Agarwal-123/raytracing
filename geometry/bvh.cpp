#include "bvh.h"

#include "collision_info.h"

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

std::shared_ptr<bvh> make_leaf_bvh(const std::vector<hittable_info> &scene, aabb bbox, const int st, const int en) {
    std::vector<int> hittable_indices;
    for (int i = st; i < en; i++) hittable_indices.push_back(scene[i].idx);
    return std::make_shared<bvh>(bbox, hittable_indices);
}

struct Bin {
    int count = 0;
    aabb bbox;
};

std::shared_ptr<bvh> make_bvh(std::vector<hittable_info> &scene, const int st, const int en) {
    const int cnt = en - st;

    aabb full_bounds;
    aabb centroid_bounds;
    for (int i = st; i < en; i++) {
        full_bounds.combine(scene[i].bbox);
        centroid_bounds.combine(scene[i].centroid);
    }

    if (cnt <= 1) return make_leaf_bvh(scene, full_bounds, st, en);

    int split_axis = centroid_bounds.longest_axis();
    double axis_width = centroid_bounds.high[split_axis] - centroid_bounds.low[split_axis];

    if (axis_width == 0.0) return make_leaf_bvh(scene, full_bounds, st, en);

    const int BINS_CNT = 16;
    Bin bins[BINS_CNT];
    for (int i = st; i < en; i++) {
        double off = scene[i].centroid[split_axis] - centroid_bounds.low[split_axis];
        int bin_idx = static_cast<int>((off/axis_width) * BINS_CNT);
        bin_idx = std::max(0, std::min(BINS_CNT-1, bin_idx));
        bins[bin_idx].count++;
        bins[bin_idx].bbox.combine(scene[i].bbox);
    }

    Bin left_bins[BINS_CNT - 1];

    Bin cur_left_bin;
    for (int i = 0; i < BINS_CNT - 1; i++) {
        cur_left_bin.count += bins[i].count;
        cur_left_bin.bbox.combine(bins[i].bbox);

        left_bins[i] = cur_left_bin;
    }

    double overall_surface_area = full_bounds.surface_area();
    int best_right_idx = -1;
    double best_cost = 1e20;

    Bin cur_right_bin;
    for (int i = BINS_CNT - 1; i > 0; i--) {
        cur_right_bin.count += bins[i].count;
        cur_right_bin.bbox.combine(bins[i].bbox);

        int li = i-1;
        double cost = 0.5 + (left_bins[li].bbox.surface_area() * left_bins[li].count +
            cur_right_bin.bbox.surface_area() * cur_right_bin.count) / overall_surface_area;
        if (best_right_idx == -1 || cost < best_cost) {
            best_right_idx = i;
            best_cost = cost;
        }
    }

    double leaf_cost = cnt;
    if (leaf_cost <= best_cost) return make_leaf_bvh(scene, full_bounds, st, en);

    auto split_it = std::partition(scene.begin() + st, scene.begin() + en,
        [&](const hittable_info &hit_info) {
            double off = hit_info.centroid[split_axis] - centroid_bounds.low[split_axis];
            int bin_idx = static_cast<int>(off/axis_width * BINS_CNT);
            bin_idx = std::max(0, std::min(BINS_CNT-1, bin_idx));
            return bin_idx < best_right_idx;
        });
    int best_split = static_cast<int>(std::distance(scene.begin(), split_it));
    if (best_split == st || best_split == en) return make_leaf_bvh(scene, full_bounds, st, en);

    return std::make_shared<bvh>(full_bounds, make_bvh(scene, st, best_split), make_bvh(scene, best_split, en));
}

struct BVHStackNode {
    const bvh* node;
    double t_enter;
};

bool hit_bvh(const std::vector<std::shared_ptr<hittable>> &scene, const std::shared_ptr<bvh> &root, ray &r, collision_info &hit_info) {
    int total_cnt = 0;

    BVHStackNode node_stack[128];
    int stack_idx = 0;

    double t_root_enter;
    if (!root->bounding_box.hit(r, t_root_enter)) {
        // std::cout << ++total_cnt << std::endl;
        return false;
    }
    ++total_cnt;
    node_stack[stack_idx++] = {root.get(), t_root_enter};

    bool hit_anything = false;
    double closest_so_far = 1e20;

    while (stack_idx > 0) {
        auto [node, t_enter] = node_stack[--stack_idx];

        if (t_enter >= closest_so_far) {
            continue;
        }

        if (node->leaf_node) {
            collision_info temp_hit_info;
            for (const int idx : node->hittable_indices) {
                if (scene[idx]->hit(r, temp_hit_info)) {
                    if (temp_hit_info.leaving != (r.get_mat_stack().empty() || r.get_mat_stack().top() != temp_hit_info.texture)) {
                        if (temp_hit_info.distance < closest_so_far) {
                            hit_info = temp_hit_info;
                            hit_anything = true;
                            closest_so_far = hit_info.distance;
                        }
                    }
                }
                ++total_cnt;
            }
            continue;
        }

        double t_left;
        double t_right;
        bool hit_left = node->left_child->bounding_box.hit(r, t_left);
        ++total_cnt;
        bool hit_right = node->right_child->bounding_box.hit(r, t_right);
        ++total_cnt;

        if (hit_left && hit_right) {
            if (t_left < t_right) {
                node_stack[stack_idx++] = {node->right_child.get(), t_right};
                node_stack[stack_idx++] = {node->left_child.get(), t_left};
            } else {
                node_stack[stack_idx++] = {node->left_child.get(), t_left};
                node_stack[stack_idx++] = {node->right_child.get(), t_right};
            }
        } else if (hit_left) {
            node_stack[stack_idx++] = {node->left_child.get(), t_left};
        } else if (hit_right) {
            node_stack[stack_idx++] = {node->right_child.get(), t_right};
        }
    }
    // std::cout << total_cnt << std::endl;
    return hit_anything;
}
