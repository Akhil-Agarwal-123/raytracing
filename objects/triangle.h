#ifndef RAYTRACING_TRIANGLE_H
#define RAYTRACING_TRIANGLE_H

#include "hittable.h"


class triangle : public hittable {
public:
    vec3 p1, p2, p3, n_p1, n_p2, n_p3;

    triangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& n_p1, const vec3& n_p2, const vec3& n_p3, const std::shared_ptr<material>& mat);

    bool hit(ray& r, collision_info& hit_info) const override;
    vec3 interpolate_outward_normal(float u, float v) const;

    aabb get_bounding_box() const override;
    vec3 centroid() const override;
    float surface_area() const;
};


#endif //RAYTRACING_TRIANGLE_H
