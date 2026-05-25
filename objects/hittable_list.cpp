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
    if (max_bounces == 0) return {}; // black

    bool got_result = false;
    collision_info hit_info {.distance = 1e8};
    collision_info hit_info_temp;
    for (const auto &h : hittables) {
        if (h->hit(r, hit_info_temp)) {
            if (!got_result || hit_info_temp.distance < hit_info.distance) {
                std::swap(hit_info, hit_info_temp);
                got_result = true;
            }
        }
    }

    if (!got_result) return {}; // black
    return hit_info.texture->get_color(*this, r, hit_info, max_bounces-1);
}
