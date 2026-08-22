#include "aabb.h"

#include <cmath>
#include <algorithm>

aabb::aabb() : trivial(true), low(0.0f, 0.0f, 0.0f), high(0.0f, 0.0f, 0.0f) {}

aabb::aabb(const vec3& low, const vec3& high)
    : trivial(false), low(low), high(high) {}

bool aabb::hit(const ray& incoming_ray, float &distance) const {
    if (trivial) return false;

    float l = -1e20f;
    float r =  1e20f;

    for (int d = 0; d < 3; ++d) {
        if (fabsf(incoming_ray.dir.e[d]) < 1e-8f) {
            // Ray parallel to slab; check origin
            if (incoming_ray.orig.e[d] < low.e[d] || incoming_ray.orig.e[d] > high.e[d]) {
                return false;
            }
        } else {
            float inv_d = 1.0f / incoming_ray.dir.e[d];
            float t0 = (low.e[d] - incoming_ray.orig.e[d]) * inv_d;
            float t1 = (high.e[d] - incoming_ray.orig.e[d]) * inv_d;

            if (inv_d < 0.0f) {
                std::swap(t0, t1);
            }

            l = fmaxf(l, t0);
            r = fminf(r, t1);

            if (r <= l) {
                return false;
            }
        }
    }

    if (r <= 0.001f) {
        return false;
    }

    distance = fmaxf(0.0f, l);
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

float aabb::surface_area() const {
    if (trivial) return 0.0f;
    const vec3 d = high - low;
    return 2.0f * (d.x() * d.y() + d.x() * d.z() + d.y() * d.z());
}

int aabb::longest_axis() const {
    vec3 extent = high - low;
    if (extent.x() > extent.y() && extent.x() > extent.z()) {
        return 0;
    }
    if (extent.y() > extent.z()) {
        return 1;
    }
    return 2;
}
