#include "box.h"

#include "../geometry/collision_info.h"

box::box(const vec3& low, const vec3& high, const std::shared_ptr<material> &t) : low(low), high(high) {
    mat = t;
}

bool box::hit(ray& r, collision_info& hit_info) {
    int causal_dim_le = -1, causal_dim_ri = -1;
    double le = -1e20, ri = 1e20;
    for (int d = 0; d < 3; d++) {
        if (r.dir.e[d] == 0) {
            if (r.orig.e[d] < low.e[d] || r.orig.e[d] > high.e[d]) {
                return false;
            }
        } else {
            double tt1 = (low.e[d] - r.orig.e[d])/r.dir.e[d];
            double tt2 = (high.e[d] - r.orig.e[d])/r.dir.e[d];
            const double t1 = std::min(tt1, tt2);
            const double t2 = std::max(tt1, tt2);
            if (t1 > le) {
                causal_dim_le = d;
                le = t1;
            }
            if (t2 < ri) {
                causal_dim_ri = d;
                ri = t2;
            }
        }
    }
    if (ri <= 0 || le > ri) return false;
    hit_info.contact_point = r.at(le <= 0 ? ri : le);
    hit_info.distance = le <= 0 ? ri : le;
    vec3 n;
    int causal_dim = le <= 0 ? causal_dim_ri : causal_dim_le;
    if (std::abs(hit_info.contact_point.e[causal_dim] - low.e[causal_dim]) < std::abs(hit_info.contact_point.e[causal_dim] - high.e[causal_dim])) {
        n.e[causal_dim] = -1;
    } else {
        n.e[causal_dim] = 1;
    }
    hit_info.set_normal(r, n);
    hit_info.texture = mat;
    // std::cout << hit_info.distance << " " << hit_info.leaving << std::endl;
    return true;
}

aabb box::get_bounding_box() const {
    return {low, high};
}

vec3 box::centroid() const {
    return (low + high)/2;
}
