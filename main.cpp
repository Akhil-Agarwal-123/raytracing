#include <iostream>
#include <memory>
#include <fstream>

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/output_image.h"
#include "objects/box.h"
#include "objects/sphere.h"
#include "util/rng.h"

void scene1(hittable_list &scene) {
    scene.add_hittable(std::make_shared<sphere>(3.0, vec3(-5.0, 3.0, -40.0), std::make_shared<dielectric>(
        0.2 * color(3.0, .5, 0.0), 0.0, 1.5)));
    scene.add_hittable(std::make_shared<sphere>(2.5, vec3(7.0, 2.5, -43.0), std::make_shared<dielectric>(
        0.1 * color(0.0, 1.0, .2), 0.0, 1.5)));
    scene.add_hittable(std::make_shared<sphere>(2.0, vec3(4.0, 6.0, -37.0), std::make_shared<dielectric>(
        0.3 * color(1.0, 0.0, 2.0), 0.0, 1.5)));

    // clear ball with blue sphere inside
    scene.add_hittable(std::make_shared<sphere>(2.0, vec3(2.0, 2.0, -33.0), std::make_shared<dielectric>(
        color(0.1, 0.1, 0.1), 0.0, 1.5)));
    scene.add_hittable(std::make_shared<sphere>(1.5, vec3(2.0, 2.0, -33.0), std::make_shared<dielectric>(
        2 * color(8.0, 2.0, 0), 0.0, 1.0)));

    // red box with blue sphere inside
    scene.add_hittable(std::make_shared<box>(vec3(10.0, 0.0, -35.0), vec3(14.0, 4.0, -31.0), std::make_shared<dielectric>(
        color(0.0, 2.0, 1.0), 0.0, 1.5)));
    scene.add_hittable(std::make_shared<sphere>(1.5, vec3(12.0, 2.0, -33.0), std::make_shared<dielectric>(
        2 * color(8.0, 2.0, 0), 0.0, 1.0)));

    scene.add_hittable(std::make_shared<sphere>(5.0, vec3(0.0, 5.0, -50.0), std::make_shared<lambertian>(color(.21, .0, .25))));
    scene.add_hittable(std::make_shared<sphere>(2.0, vec3(-8.0, 10.0, -55.0), std::make_shared<metal>(color(.2, .2, .2))));
    scene.add_hittable(std::make_shared<sphere>(3.0, vec3(-13.0, 7.0, -50.0), std::make_shared<metal>(color(.4, .4, .2))));

    // the ground
    scene.add_hittable(std::make_shared<sphere>(1e4, vec3(0.0, -1e4, 0.0), std::make_shared<lambertian>(color(0.8, 0.8, 0.8))));

    // // light source all around
    // scene.add_hittable(std::make_shared<sphere>(3e4, vec3(0.0, -1e4, 0.0), std::make_shared<light_source>(color(1.0, 1.0, 1.0))));

    // light source above objects
    scene.add_hittable(std::make_shared<sphere>(30, vec3(5.0, 45.0, -40.0), std::make_shared<light_source>(color(1.0, 1.0, 1.0))));
}

void setup_bvh_stress_test_scene(hittable_list& scene) {
    // 1. The Ground Plane
    scene.add_hittable(std::make_shared<sphere>(
        1e5, vec3(0.0, -1e5, 0.0), std::make_shared<lambertian>(color(0.5, 0.5, 0.5))
    ));

    // 2. Large Central Showcase Objects (to anchor the composition)
    scene.add_hittable(std::make_shared<sphere>(
        3.0, vec3(0.0, 3.0, -40.0), std::make_shared<dielectric>(color(8.0, 0, 2.0), 0.0, 1.5)
    ));
    scene.add_hittable(std::make_shared<sphere>(
        3.0, vec3(-7.0, 3.0, -45.0), std::make_shared<metal>(color(0.7, 0.6, 0.5))
    ));
    scene.add_hittable(std::make_shared<sphere>(
        3.0, vec3(7.0, 3.0, -45.0), std::make_shared<lambertian>(color(0.2, 0.4, 0.7))
    ));

    // 3. Dense Procedural Grid of Small Spheres
    // Spans from X = -20 to 20 and Z = -65 to -20
    for (int x = -20; x <= 20; x += 2) {
        for (int z = -65; z <= -20; z += 2) {
            double choose_mat = rng::random_double();

            // Generate center point with a small pseudo-random spatial jitter
            vec3 center(
                x + 0.8 * rng::random_double(-0.5, 0.5),
                0.55,
                z + 0.8 * rng::random_double(-0.5, 0.5)
            );

            // Safety Check: Avoid spawning small spheres inside our large showcase objects
            if ((center - vec3(0.0, 0.5, -40.0)).length() > 4.5 &&
                (center - vec3(-7.0, 0.5, -45.0)).length() > 4.5 &&
                (center - vec3(7.0, 0.5, -45.0)).length() > 4.5) {

                std::shared_ptr<material> sphere_mat;

                if (choose_mat < 0.30) {
                    // 30% Chance: Lambertian (Diffuse) with a rich color profile
                    color albedo = rng::random_vec3() * rng::random_vec3();
                    sphere_mat = std::make_shared<lambertian>(albedo);
                }
                else if (choose_mat < 0.55) {
                    // 25% Chance: Metal with random high reflectivity colors
                    color albedo = rng::random_vec3(0.5, 1.0);
                    sphere_mat = std::make_shared<metal>(albedo);
                }
                else {
                    // 45% Chance: Smooth Dielectric (Glass) with subtle absorption properties
                    color absorb = rng::random_vec3(0.0, 1.0) * 4;
                    sphere_mat = std::make_shared<dielectric>(absorb, 0.0, 1.5);
                }

                scene.add_hittable(std::make_shared<sphere>(0.55, center, sphere_mat));
            }
        }
    }

    // 4. Large Overhead Area Light Source
    scene.add_hittable(std::make_shared<sphere>(
        30.0, vec3(5.0, 50.0, -45.0), std::make_shared<light_source>(color(1.0, 1.0, 1.0))
    ));
}

int main() {
    camera cam(vec3(0.0, 5.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), 40.0, 1920, 1080);
    hittable_list scene;

    setup_bvh_stress_test_scene(scene);
    // scene1(scene);

    scene.mark_done();

    std::cout << "Done constructing BVH" << std::endl;

    auto img = cam.capture_image(scene);

    std::ofstream fout("/home/akhil/raytracing/image.ppm");
    write_image(fout, img);
    fout.close();
}
