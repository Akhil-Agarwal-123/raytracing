#include "box.h"

#include <cmath>
#include <algorithm>
#include "../geometry/collision_info.h"

box::box(const vec3& low, const vec3& high, const std::shared_ptr<material>& t)
    : low(low), high(high) {
    mat = t;
}

bool box::hit(ray& r, collision_info& hit_info) const {
    int causal_dim_le = -1;
    int causal_dim_ri = -1;
    float le = -1e20f;
    float ri =  1e20f;

    for (int d = 0; d < 3; ++d) {
        if (fabsf(r.dir.e[d]) < 1e-8f) {
            // Ray is parallel to this axis slab; check if origin lies outside
            if (r.orig.e[d] < low.e[d] || r.orig.e[d] > high.e[d]) {
                return false;
            }
        } else {
            float inv_d = 1.0f / r.dir.e[d];
            float t0 = (low.e[d] - r.orig.e[d]) * inv_d;
            float t1 = (high.e[d] - r.orig.e[d]) * inv_d;

            if (inv_d < 0.0f) {
                std::swap(t0, t1);
            }

            if (t0 > le) {
                le = t0;
                causal_dim_le = d;
            }
            if (t1 < ri) {
                ri = t1;
                causal_dim_ri = d;
            }

            if (ri <= le) {
                return false;
            }
        }
    }

    if (ri <= 0.001f) {
        return false;
    }

    bool hit_near = (le > 0.001f);
    float hit_t = hit_near ? le : ri;
    int causal_dim = hit_near ? causal_dim_le : causal_dim_ri;

    hit_info.distance = hit_t;
    hit_info.contact_point = r.at(hit_t);

    // Compute the true outward face normal
    vec3 outward_normal(0.0f, 0.0f, 0.0f);
    if (hit_near) {
        // Ray entered through near slab face: outward normal opposes ray direction along this axis
        outward_normal.e[causal_dim] = (r.dir.e[causal_dim] > 0.0f) ? -1.0f : 1.0f;
    } else {
        // Ray exited through far slab face: outward normal aligns with ray direction along this axis
        outward_normal.e[causal_dim] = (r.dir.e[causal_dim] > 0.0f) ? 1.0f : -1.0f;
    }

    hit_info.set_normal(r, outward_normal);
    hit_info.texture = mat;
    hit_info.has_volume = true;
    return true;
}

aabb box::get_bounding_box() const {
    return {low, high};
}

vec3 box::centroid() const {
    return (low + high) * 0.5f;
}
