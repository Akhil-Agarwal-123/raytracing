#ifndef RAYTRACING_CAMERA_H
#define RAYTRACING_CAMERA_H

#include "../geometry/vec3.h"
#include "output_image.h"
#include "../objects/hittable_list.h"

class camera {
public:
    camera(vec3 origin, vec3 forward, vec3 up, double fov, int horizontal_resolution, int vertical_resolution);

    output_image capture_image(const hittable_list &scene) const;

private:
    vec3 origin, forward, up, right;
    double viewport_distance;
    int horizontal_resolution, vertical_resolution;
};

#endif //RAYTRACING_CAMERA_H
