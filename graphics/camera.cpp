#include "camera.h"

#include <execution>
#include <map>

#include "../geometry/vec3.h"
#include <ranges>
#include <vector>

#include "output_image.h"
#include "../geometry/collision_info.h"
#include "../geometry/ray.h"
#include "../objects/hittable_list.h"
#include "../util/rng.h"

camera::camera(vec3 origin, vec3 forward, vec3 up, double fov, int horizontal_resolution, int vertical_resolution)
    : origin(origin), forward(forward), up(up), horizontal_resolution(horizontal_resolution),
    vertical_resolution(vertical_resolution) {
    right = cross(forward, up);
    viewport_distance = (vertical_resolution / 2.0) / tan(fov * std::numbers::pi/180 / 2);
}

output_image camera::capture_image(const hittable_list &scene) const {
    output_image img(vertical_resolution, horizontal_resolution);

    ray test_ray(origin, {0, 0, 1}, {});
    std::map<double, std::shared_ptr<material>> mats;
    collision_info cur_collision_info;
    for (auto &h : scene.hittables) {
        if (h->hit(test_ray, cur_collision_info)) {
            if (cur_collision_info.leaving) mats[cur_collision_info.distance] = cur_collision_info.texture;
        }
    }

    std::stack<std::shared_ptr<material>> startingPoint;
    for (auto & [k, v] : std::views::reverse(mats)) {
        startingPoint.push(v);
    }

    // #pragma omp parallel for shared(img, scene) schedule(dynamic, 8)
    for (int i = 0; i < vertical_resolution; i++) {
        auto& local_rng = rng::get_state();

        for (int j = 0; j < horizontal_resolution; j++) {
            color c;

            int valid_cnt = 0;
            for (int z = 0; z < 100; z++) {
                // Directly pull from the local_rng object on the stack
                double rand_i = local_rng.dist(local_rng.engine);
                double rand_j = local_rng.dist(local_rng.engine);

                const ray r(origin, unit_vector(
                    forward * viewport_distance
                    + up * (vertical_resolution/2.0 - (i + rand_i))
                    + right * (-horizontal_resolution/2.0 + (j + rand_j))), startingPoint);

                color x = scene.get_raytraced_color(r, 20);
                if (x.x() >= 0.0 && x.x() <= 1.0 && x.y() >= 0.0 && x.y() <= 1.0) {
                    valid_cnt++;
                    c += x;
                }
            }
            c /= valid_cnt;
            img.set_pixel(j, i, c);
        }
    }
    return img;
}
