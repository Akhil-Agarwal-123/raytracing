#include "triangle.h"

#include <cmath>
#include "../geometry/collision_info.h"

triangle::triangle(const vec3& p1, const vec3& p2, const vec3& p3,
                   const vec3& n_p1, const vec3& n_p2, const vec3& n_p3,
                   const std::shared_ptr<material>& mat)
    : p1(p1), p2(p2), p3(p3), n_p1(n_p1), n_p2(n_p2), n_p3(n_p3) {
    this->mat = mat;
}

bool triangle::hit(ray& r, collision_info& hit_info) const {
    // Möller–Trumbore ray-triangle intersection
    vec3 e1 = p2 - p1;
    vec3 e2 = p3 - p1;
    vec3 p = cross(r.dir, e2);
    float pe1 = dot(p, e1);

    if (fabsf(pe1) < 1e-8f) {
        return false;
    }

    float inv_pe1 = 1.0f / pe1;
    vec3 t_o = r.orig - p1;

    float u = dot(p, t_o) * inv_pe1;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    vec3 q = cross(t_o, e1);
    float v = dot(q, r.dir) * inv_pe1;
    if (v < 0.0f || (u + v) > 1.0f) {
        return false;
    }

    float t = dot(q, e2) * inv_pe1;
    if (t < 0.001f) {
        return false;
    }

    hit_info.distance = t;
    hit_info.texture = this->mat;
    hit_info.contact_point = r.at(t);

    vec3 geo_normal = unit_vector(cross(e1, e2));
    hit_info.set_normal(r, interpolate_outward_normal(u, v), geo_normal);
    hit_info.has_volume = false;
    return true;
}

vec3 triangle::interpolate_outward_normal(const float u, const float v) const {
    float alpha = 1.0f - u - v;
    return unit_vector(alpha * n_p1 + u * n_p2 + v * n_p3);
}

aabb triangle::get_bounding_box() const {
    vec3 min_pt = min(p1, min(p2, p3));
    vec3 max_pt = max(p1, max(p2, p3));

    // Pad flat triangles so the AABB has non-zero volume in all 3 dimensions
    constexpr float eps = 1e-4f;
    for (int i = 0; i < 3; ++i) {
        if (max_pt.e[i] - min_pt.e[i] < eps) {
            min_pt.e[i] -= eps;
            max_pt.e[i] += eps;
        }
    }

    return { min_pt, max_pt };
}

vec3 triangle::centroid() const {
    return (p1 + p2 + p3) * (1.0f / 3.0f);
}

float triangle::surface_area() const {
    return cross(p2 - p1, p3 - p1).length() * 0.5f;
}