#include "hittable_list.h"

#include <memory>
#include <vector>
#include <algorithm>
#include "../geometry/ray.h"
#include "../graphics/color.h"
#include "../geometry/collision_info.h"
#include "../util/rng.h"

void hittable_list::add_hittable(const std::shared_ptr<hittable>& hittable) {
    if (hittable) {
        hittables.push_back(hittable);
    }
}

color hittable_list::get_raytraced_color(ray r, int max_bounces, rng::Sampler& sampler) const {
    color c(1.0f, 1.0f, 1.0f);

    for (int bounce = 0; bounce < max_bounces; ++bounce) {
        collision_info hit_info;
        if (!hit_bvh(hittables, bounding_volume_hierarchy, r, hit_info)) {
            return color(0.0f, 0.0f, 0.0f); // Miss / background
        }

        if (hit_info.has_volume) {
            // Check if our material stack thinks we are currently inside this object
            bool is_inside_mat = (!r.get_mat_stack().empty() && r.get_mat_stack().back() == hit_info.texture);
            bool expected_leaving = is_inside_mat;

            if (hit_info.leaving != expected_leaving) {
                // Invalid State: Treat as transparent ghost ray and step forward
                constexpr float EPSILON = 1e-4f;
                r.orig = hit_info.contact_point + r.dir * EPSILON;
                continue;
            }
        }

        color albedo;
        vec3 next_dir;
        bool reflecting = false;

        if (hit_info.texture->get_next_ray(r, hit_info, albedo, next_dir, reflecting, sampler)) {
            if (hit_info.has_volume && !reflecting) {
                if (hit_info.leaving) {
                    r.remove_last_mat();
                } else {
                    r.add_new_mat(hit_info.texture);
                }
            }

            constexpr float NORMAL_OFFSET = 1e-3f;
            if (reflecting) {
                r.orig = hit_info.contact_point + NORMAL_OFFSET * hit_info.geometric_normal;
            } else {
                r.orig = hit_info.contact_point - NORMAL_OFFSET * hit_info.geometric_normal;
            }

            r.dir = next_dir;
            c *= albedo;

            // Russian Roulette early termination past bounce 4
            if (bounce > 4) {
                float max_c = fmaxf(c.e[0], fmaxf(c.e[1], c.e[2]));
                float survive_prob = fmaxf(0.05f, fminf(max_c, 0.95f));
                if (rng::random_float(sampler) > survive_prob) {
                    break;
                }
                c *= (1.0f / survive_prob);
            }
        } else {
            // Emissive / light source hit
            return c * albedo;
        }
    }

    return color(0.0f, 0.0f, 0.0f);
}

void hittable_list::mark_done() {
    bounding_volume_hierarchy = make_bvh(hittables);
}
