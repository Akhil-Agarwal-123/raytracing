#ifndef RAYTRACING_MESH_H
#define RAYTRACING_MESH_H

#include "hittable.h"
#include <vector>

class bvh;

class mesh : public hittable {
public:
    std::shared_ptr<bvh> bounding_volume_hierarchy;
    std::vector<std::shared_ptr<hittable>> triangles;
    vec3 center;

    mesh(const std::string &obj_file, const std::shared_ptr<material> &mat);

    bool hit(ray& r, collision_info& hit_info) const override;

    aabb get_bounding_box() const override;
    vec3 centroid() const override;
};


#endif //RAYTRACING_MESH_H
