#include "hittable_list.h"

#include <memory>
#include <vector>
#include "../geometry/ray.h"
#include "../graphics/color.h"
#include "../geometry/collision_info.h"

void hittable_list::add_hittable(const std::shared_ptr<hittable>& hittable) {
    if (hittable) {
        hittables.push_back(hittable);
    }
}

color hittable_list::get_raytraced_color(ray r, int max_bounces) const {
    color c = {1.0, 1.0, 1.0};

    for (int bounce = 0; bounce < max_bounces; bounce++) {
        collision_info hit_info;
        if (!hit_bvh(hittables, bounding_volume_hierarchy, r, hit_info)) {
            return {};
        }

        color albedo;
        vec3 next_dir;
        bool reflecting;
        if (hit_info.texture->get_next_ray(r, hit_info, albedo, next_dir, reflecting)) {
            if (hit_info.has_volume && !reflecting) {
                if (hit_info.leaving) r.remove_last_mat();
                else r.add_new_mat(hit_info.texture);
            }
            r.orig = hit_info.contact_point;
            if (reflecting) {
                r.orig += 1e-3 * hit_info.normal;
            } else {
                r.orig -= 1e-3 * hit_info.normal;
            }
            r.dir = next_dir;
            c *= albedo;
        } else {
            return c * albedo;
        }
    }
    return {};
}

void hittable_list::mark_done() {
    bounding_volume_hierarchy = make_bvh(hittables);
}
