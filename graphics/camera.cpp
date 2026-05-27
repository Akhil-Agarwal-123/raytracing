#include "camera.h"

#include <execution>
#include <map>
#include <ranges>
#include <vector>
#include <atomic>   // Added for thread-safe row counting
#include <iostream> // Added for progress logging via std::clog

#include "../geometry/vec3.h"
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

    // --- Progress Bar Configurations ---
    const int bar_width = 40;
    std::atomic<int> completed_rows(0);

    #pragma omp parallel for shared(img, scene, completed_rows) schedule(dynamic, 8)
    for (int i = 0; i < vertical_resolution; i++) {
        auto& local_rng = rng::get_state();

        for (int j = 0; j < horizontal_resolution; j++) {
            color c;

            int samples_per_pixel = 200;
            for (int z = 0; z < samples_per_pixel; z++) {
                // Directly pull from the local_rng object on the stack
                double rand_i = local_rng.dist(local_rng.engine);
                double rand_j = local_rng.dist(local_rng.engine);

                const ray r(origin, unit_vector(
                    forward * viewport_distance
                    + up * (vertical_resolution/2.0 - (i + rand_i))
                    + right * (-horizontal_resolution/2.0 + (j + rand_j))), startingPoint);

                color x = scene.get_raytraced_color(r, 10);
                c += x;
            }
            c /= samples_per_pixel;
            img.set_pixel(j, i, c);
        }

        // --- Thread-Safe Progress Bar Update ---
        int current_completed = ++completed_rows;

        // Prevent multiple threads from jumping into console output simultaneously
        #pragma omp critical
        {
            double progress = static_cast<double>(current_completed) / vertical_resolution;
            int pos = static_cast<int>(bar_width * progress);

            std::clog << "\r[";
            for (int k = 0; k < bar_width; ++k) {
                if (k < pos) std::clog << "=";
                else if (k == pos) std::clog << ">";
                else std::clog << " ";
            }
            std::clog << "] " << static_cast<int>(progress * 100.0) << "%" << std::flush;
        }
    }

    // Clean up terminal formatting when done
    std::clog << "\nRender Complete.\n" << std::flush;

    return img;
}