#ifndef RAYTRACING_BOX_H
#define RAYTRACING_BOX_H
#include "hittable.h"


class box : public hittable {
public:
    box(const vec3& low, const vec3& high, const std::shared_ptr<material> &t);
    bool hit(ray& r, collision_info& hit_info) override;

private:
    vec3 low, high;
};


#endif //RAYTRACING_BOX_H
