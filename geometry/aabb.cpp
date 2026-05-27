#include "aabb.h"

aabb::aabb() {
    trivial = true;
}

aabb::aabb(const vec3& low, const vec3& high) : trivial(false), low(low), high(high) {}

bool aabb::hit(const ray& incoming_ray, double &distance) const {
    if (trivial) return false;

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
    if (r <= 0 || l > r) return false;
    distance = std::max(0.0, l);
    return true;
}

void aabb::combine(const aabb& other) {
    if (other.trivial) return;

    if (trivial) {
        low = other.low;
        high = other.high;
        trivial = false;
    } else {
        low = min(low, other.low);
        high = max(high, other.high);
    }
}

void aabb::combine(const vec3& other) {
    if (trivial) {
        low = other;
        high = other;
        trivial = false;
    } else {
        low = min(low, other);
        high = max(high, other);
    }
}

double aabb::surface_area() const {
    const vec3 d = high - low;
    return 2 * (d.x() * d.y() + d.x() * d.z() + d.y() * d.z());
}

int aabb::longest_axis() const {
    int d = -1;
    double mx = -1;
    for (int i = 0; i < 3; i++) {
        if (d == -1 || high.e[i] - low.e[i] > mx) d = i;
    }
    return d;
}
