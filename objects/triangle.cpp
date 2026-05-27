#include "triangle.h"
#include "../geometry/collision_info.h"

triangle::triangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& n_p1, const vec3& n_p2, const vec3& n_p3,
    const std::shared_ptr<material>& mat) : p1(p1), p2(p2), p3(p3), n_p1(n_p1), n_p2(n_p2), n_p3(n_p3) {
    this->mat = mat;
}

bool triangle::hit(ray& r, collision_info& hit_info) const {
    // using moller-trumbore
    vec3 e1 = p2 - p1;
    vec3 e2 = p3 - p1;
    vec3 p = cross(r.dir, e2);
    double pe1 = dot(p, e1);
    if (std::abs(pe1) < 1e-8) return false;
    vec3 t_o = r.orig - p1;
    double u = dot(p, t_o)/pe1;
    if (u < 0 || u > 1) return false;
    vec3 q = cross(t_o, e1);
    double v = dot(q, r.dir)/pe1;
    if (v < 0 || u + v > 1) return false;
    double t = dot(q, e2)/pe1;
    if (t < 0) return false;

    hit_info.distance = t;
    hit_info.texture = this->mat;
    hit_info.contact_point = r.at(t);
    hit_info.set_normal(r, interpolate_outward_normal(u, v), unit_vector(cross(e1, e2)));
    hit_info.has_volume = false;
    return true;
}

vec3 triangle::interpolate_outward_normal(const double u, const double v) const {
    double alpha = 1 - u - v;
    double beta = u;
    double gamma = v;
    return unit_vector(alpha * n_p1 + beta * n_p2 + gamma * n_p3);
}

aabb triangle::get_bounding_box() const {
    return { min(p1, min(p2, p3)), max(p1, max(p2, p3)) };
}

vec3 triangle::centroid() const {
    return (p1 + p2 + p3)/3;
}

double triangle::surface_area() const {
    return cross(p2 - p1, p3 - p1).length()/2;
}
