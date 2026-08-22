#include "camera.h"

#include <map>
#include <ranges>
#include <vector>
#include <atomic>
#include <iostream>
#include <cmath>
#include <omp.h>

#include "../geometry/vec3.h"
#include "output_image.h"
#include "../geometry/collision_info.h"
#include "../geometry/ray.h"
#include "../objects/hittable_list.h"
#include "../util/rng.h"

#ifndef PI_CONST
#define PI_CONST 3.14159265358979323846f
#endif

camera::camera(const vec3 origin, const vec3 forward, const vec3 up, const float fov, const int horizontal_resolution, const int vertical_resolution)
    : origin(origin), forward(forward), up(up), horizontal_resolution(horizontal_resolution),
      vertical_resolution(vertical_resolution) {
    right = cross(forward, up);
    constexpr float deg_to_rad = PI_CONST / 180.0f;
    viewport_distance = (static_cast<float>(vertical_resolution) * 0.5f) / tanf(fov * deg_to_rad * 0.5f);
}

output_image camera::capture_image(const hittable_list &scene) const {
    output_image img(vertical_resolution, horizontal_resolution);

    ray test_ray(origin, vec3(0.0f, 0.0f, 1.0f), {});
    std::map<float, std::shared_ptr<material>> mats;
    for (auto &h : scene.hittables) {
        collision_info cur_collision_info;
        if (h->hit(test_ray, cur_collision_info)) {
            if (cur_collision_info.leaving) {
                mats[cur_collision_info.distance] = cur_collision_info.texture;
            }
        }
    }

    std::vector<std::shared_ptr<material>> startingPoint;
    for (auto & [k, v] : std::views::reverse(mats)) {
        startingPoint.push_back(v);
    }

    // --- Progress Bar Configurations ---
    constexpr int bar_width = 40;
    const int total_rows = vertical_resolution;
    const int progress_step = (total_rows > 100) ? (total_rows / 100) : 1;
    std::atomic completed_rows(0);

    // --- Precomputed Loop Invariants ---
    const float half_h = static_cast<float>(vertical_resolution) * 0.5f;
    const float half_w = static_cast<float>(horizontal_resolution) * 0.5f;
    constexpr int samples_per_pixel = 200;
    constexpr float inv_spp = 1.0f / static_cast<float>(samples_per_pixel);

    #pragma omp parallel shared(img, scene, completed_rows, startingPoint)
    {
        int tid = omp_get_thread_num();
        rng::Sampler sampler(static_cast<uint32_t>(tid * 19937 + 1));

        #pragma omp for schedule(dynamic, 8)
        for (int i = 0; i < vertical_resolution; i++) {
            const float row_offset = half_h - static_cast<float>(i);

            for (int j = 0; j < horizontal_resolution; j++) {
                const float col_offset = -half_w + static_cast<float>(j);
                color c(0.0f, 0.0f, 0.0f);

                for (int z = 0; z < samples_per_pixel; z++) {
                    float rand_i = rng::random_float(sampler);
                    float rand_j = rng::random_float(sampler);

                    const ray r(origin, unit_vector(
                        forward * viewport_distance
                        + up * (row_offset - rand_i)
                        + right * (col_offset + rand_j)), startingPoint);

                    color x = scene.get_raytraced_color(r, 10, sampler);
                    c += x;
                }

                c *= inv_spp;
                img.set_pixel(j, i, c);
            }

            // --- Throttled Progress Bar Update ---
            int current_completed = ++completed_rows;

            if (current_completed % progress_step == 0 || current_completed == total_rows) {
                #pragma omp critical
                {
                    float progress = static_cast<float>(current_completed) / static_cast<float>(total_rows);
                    int pos = static_cast<int>(static_cast<float>(bar_width) * progress);

                    std::clog << "\r[";
                    for (int k = 0; k < bar_width; ++k) {
                        if (k < pos) std::clog << "=";
                        else if (k == pos) std::clog << ">";
                        else std::clog << " ";
                    }
                    std::clog << "] " << static_cast<int>(progress * 100.0f) << "%" << std::flush;
                }
            }
        }
    }

    std::clog << "\nRender Complete.\n" << std::flush;

    return img;
}