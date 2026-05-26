#include "aabb.h"

aabb::aabb(vec3 low, vec3 high) : low(low), high(high) {}

bool aabb::hit(const ray& incoming_ray) const {
    double l = -1e20, r = 1e20;
    for (int d = 0; d < 3; d++) {
        if (incoming_ray.dir.e[d] == 0) {
            if (incoming_ray.orig.e[d] < low.e[d] || incoming_ray.orig.e[d] > high.e[d]) {
                return false;
            }
        } else {
            double t1 = (low.e[d] - incoming_ray.orig.e[d])/incoming_ray.dir.e[d];
            double t2 = (high.e[d] - incoming_ray.orig.e[d])/incoming_ray.dir.e[d];
            l = std::max(l, std::min(t1, t2));
            r = std::min(r, std::max(t1, t2));
        }
    }
    return r > 0 && l <= r;
}

void aabb::combine(const aabb& other) {
    low.e[0] = std::min(low.e[0], other.low.e[0]);
    low.e[1] = std::min(low.e[1], other.low.e[1]);
    low.e[2] = std::min(low.e[2], other.low.e[2]);

    high.e[0] = std::max(high.e[0], other.high.e[0]);
    high.e[1] = std::max(high.e[1], other.high.e[1]);
    high.e[2] = std::max(high.e[2], other.high.e[2]);
}

double aabb::surface_area() const {
    const vec3 d = high - low;
    return 2 * (d.x() * d.y() + d.x() * d.z() + d.y() * d.z());
}
