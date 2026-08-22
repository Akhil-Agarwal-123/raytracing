#include <iostream>
#include <memory>
#include <fstream>
#include <string>

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/output_image.h"
#include "objects/box.h"
#include "objects/mesh.h"
#include "objects/sphere.h"
#include "objects/triangle.h"
#include "util/rng.h"

static void save_rendered_image(const std::string& filename, const output_image& img) {
    std::ofstream fout(filename);
    if (fout.is_open()) {
        write_image(fout, img);
        fout.close();
        std::cout << "Saved render to " << filename << std::endl;
    } else {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
    }
}

// -----------------------------------------------------------------------------
// 1. Dragon Mesh Scene Pipeline
// -----------------------------------------------------------------------------
void render_dragon_mesh_scene(const std::string& output_filename = "/home/akhil/raytracing/image.ppm") {
    hittable_list scene;

    auto mesh_obj = std::make_shared<mesh>(
        "/home/akhil/raytracing/dragon_smooth.obj",
        std::make_shared<dielectric>(color(8.0f, 0.0f, 2.0f), 0.0f, 1.5f)
    );
    scene.add_hittable(mesh_obj);

    vec3 mesh_center = mesh_obj->centroid();
    std::cout << "Mesh centroid: " << mesh_center << std::endl;

    vec3 cam_origin = mesh_center + vec3(-100.0f, 50.0f, 300.0f);
    vec3 cam_forward = unit_vector(mesh_center - cam_origin);
    vec3 cam_up(0.0f, 1.0f, 0.0f);

    constexpr int width = 3840 / 5;
    constexpr int height = 2160 / 5;
    camera cam(cam_origin, cam_forward, cam_up, 40.0f, width, height);

    scene.add_hittable(std::make_shared<sphere>(
        100.0f,
        mesh_center + 1.3f * vec3(-50.0f, 150.0f, 50.0f),
        std::make_shared<light_source>(color(1.0f, 1.0f, 1.0f))
    ));

    scene.add_hittable(std::make_shared<sphere>(
        1e5f,
        mesh_center + vec3(0.0f, -1e5f - 80.0f, 0.0f),
        std::make_shared<lambertian>(color(0.5f, 0.5f, 0.5f))
    ));

    scene.mark_done();
    std::cout << "Done constructing BVH for Dragon Scene" << std::endl;

    auto img = cam.capture_image(scene);
    save_rendered_image(output_filename, img);
}

// -----------------------------------------------------------------------------
// 2. Sphere and Box Test Scene Pipeline
// -----------------------------------------------------------------------------
void render_sphere_and_box_scene(const std::string& output_filename = "/home/akhil/raytracing/sphere_box.ppm") {
    hittable_list scene;

    scene.add_hittable(std::make_shared<sphere>(3.0f, vec3(-5.0f, 3.0f, -40.0f), std::make_shared<dielectric>(
        0.2f * color(3.0f, 0.5f, 0.0f), 0.0f, 1.5f)));
    scene.add_hittable(std::make_shared<sphere>(2.5f, vec3(7.0f, 2.5f, -43.0f), std::make_shared<dielectric>(
        0.1f * color(0.0f, 1.0f, 0.2f), 0.0f, 1.5f)));
    scene.add_hittable(std::make_shared<sphere>(2.0f, vec3(4.0f, 6.0f, -37.0f), std::make_shared<dielectric>(
        0.3f * color(1.0f, 0.0f, 2.0f), 0.0f, 1.5f)));

    scene.add_hittable(std::make_shared<sphere>(2.0f, vec3(2.0f, 2.0f, -33.0f), std::make_shared<dielectric>(
        color(0.1f, 0.1f, 0.1f), 0.0f, 1.5f)));
    scene.add_hittable(std::make_shared<sphere>(1.5f, vec3(2.0f, 2.0f, -33.0f), std::make_shared<dielectric>(
        2.0f * color(8.0f, 2.0f, 0.0f), 0.0f, 1.0f)));

    scene.add_hittable(std::make_shared<box>(vec3(10.0f, 0.0f, -35.0f), vec3(14.0f, 4.0f, -31.0f), std::make_shared<dielectric>(
        color(0.0f, 2.0f, 1.0f), 0.0f, 1.5f)));
    scene.add_hittable(std::make_shared<sphere>(1.5f, vec3(12.0f, 2.0f, -33.0f), std::make_shared<dielectric>(
        2.0f * color(8.0f, 2.0f, 0.0f), 0.0f, 1.0f)));

    scene.add_hittable(std::make_shared<sphere>(5.0f, vec3(0.0f, 5.0f, -50.0f), std::make_shared<lambertian>(color(0.21f, 0.0f, 0.25f))));
    scene.add_hittable(std::make_shared<sphere>(2.0f, vec3(-8.0f, 10.0f, -55.0f), std::make_shared<metal>(color(0.2f, 0.2f, 0.2f))));
    scene.add_hittable(std::make_shared<sphere>(3.0f, vec3(-13.0f, 7.0f, -50.0f), std::make_shared<metal>(color(0.4f, 0.4f, 0.2f))));

    scene.add_hittable(std::make_shared<sphere>(1e4f, vec3(0.0f, -1e4f, 0.0f), std::make_shared<lambertian>(color(0.8f, 0.8f, 0.8f))));
    scene.add_hittable(std::make_shared<sphere>(30.0f, vec3(5.0f, 45.0f, -40.0f), std::make_shared<light_source>(color(1.0f, 1.0f, 1.0f))));

    vec3 cam_origin(0.0f, 5.0f, -10.0f);
    vec3 look_at(0.0f, 3.0f, -40.0f);
    vec3 cam_forward = unit_vector(look_at - cam_origin);
    vec3 cam_up(0.0f, 1.0f, 0.0f);

    constexpr int width = 3840 / 5;
    constexpr int height = 2160 / 5;
    camera cam(cam_origin, cam_forward, cam_up, 45.0f, width, height);

    scene.mark_done();
    std::cout << "Done constructing BVH for Sphere/Box Scene" << std::endl;

    auto img = cam.capture_image(scene);
    save_rendered_image(output_filename, img);
}

// -----------------------------------------------------------------------------
// 3. Triangle Test Scene Pipeline
// -----------------------------------------------------------------------------
void render_triangle_scene(const std::string& output_filename = "/home/akhil/raytracing/triangle_scene.ppm") {
    hittable_list scene;

    scene.add_hittable(std::make_shared<sphere>(2.0f, vec3(2.0f, 2.0f, -33.0f), std::make_shared<dielectric>(
        color(0.1f, 0.1f, 0.1f), 0.0f, 1.5f)));
    scene.add_hittable(std::make_shared<sphere>(1.5f, vec3(2.0f, 2.0f, -33.0f), std::make_shared<dielectric>(
        2.0f * color(8.0f, 2.0f, 0.0f), 0.0f, 1.0f)));

    scene.add_hittable(std::make_shared<box>(vec3(10.0f, 0.0f, -35.0f), vec3(14.0f, 4.0f, -31.0f), std::make_shared<dielectric>(
        color(0.0f, 2.0f, 1.0f), 0.0f, 1.5f)));
    scene.add_hittable(std::make_shared<sphere>(1.5f, vec3(12.0f, 2.0f, -33.0f), std::make_shared<dielectric>(
        2.0f * color(8.0f, 2.0f, 0.0f), 0.0f, 1.0f)));

    scene.add_hittable(std::make_shared<triangle>(
        vec3( 0.0f,  4.0f, -22.0f),
        vec3(-2.0f,  0.0f, -20.0f),
        vec3( 2.0f,  0.0f, -20.0f),
        vec3( 0.0f,  0.0f,   1.0f),
        vec3( 0.0f,  0.0f,   1.0f),
        vec3( 0.0f,  0.0f,   1.0f),
        std::make_shared<metal>(color(0.2f, 0.2f, 0.2f))
    ));
    scene.add_hittable(std::make_shared<sphere>(2.0f, vec3(-8.0f, 10.0f, -55.0f), std::make_shared<metal>(color(0.2f, 0.2f, 0.2f))));
    scene.add_hittable(std::make_shared<sphere>(3.0f, vec3(-13.0f, 7.0f, -50.0f), std::make_shared<metal>(color(0.4f, 0.4f, 0.2f))));

    scene.add_hittable(std::make_shared<sphere>(1e4f, vec3(0.0f, -1e4f, 0.0f), std::make_shared<lambertian>(color(0.8f, 0.8f, 0.8f))));
    scene.add_hittable(std::make_shared<sphere>(30.0f, vec3(5.0f, 45.0f, -40.0f), std::make_shared<light_source>(color(1.0f, 1.0f, 1.0f))));

    vec3 cam_origin(0.0f, 4.0f, -5.0f);
    vec3 look_at(0.0f, 2.0f, -25.0f);
    vec3 cam_forward = unit_vector(look_at - cam_origin);
    vec3 cam_up(0.0f, 1.0f, 0.0f);

    constexpr int width = 3840 / 5;
    constexpr int height = 2160 / 5;
    camera cam(cam_origin, cam_forward, cam_up, 45.0f, width, height);

    scene.mark_done();
    std::cout << "Done constructing BVH for Triangle Scene" << std::endl;

    auto img = cam.capture_image(scene);
    save_rendered_image(output_filename, img);
}

// -----------------------------------------------------------------------------
// 4. BVH Stress Test Scene Pipeline
// -----------------------------------------------------------------------------
void render_bvh_stress_test_scene(const std::string& output_filename = "/home/akhil/raytracing/bvh_stress.ppm") {
    hittable_list scene;
    rng::Sampler sampler(1337);

    scene.add_hittable(std::make_shared<sphere>(
        1e5f, vec3(0.0f, -1e5f, 0.0f), std::make_shared<lambertian>(color(0.5f, 0.5f, 0.5f))
    ));

    scene.add_hittable(std::make_shared<sphere>(
        3.0f, vec3(0.0f, 3.0f, -40.0f), std::make_shared<dielectric>(color(8.0f, 0.0f, 2.0f), 0.0f, 1.5f)
    ));
    scene.add_hittable(std::make_shared<sphere>(
        3.0f, vec3(-7.0f, 3.0f, -45.0f), std::make_shared<metal>(color(0.7f, 0.6f, 0.5f))
    ));
    scene.add_hittable(std::make_shared<sphere>(
        3.0f, vec3(7.0f, 3.0f, -45.0f), std::make_shared<lambertian>(color(0.2f, 0.4f, 0.7f))
    ));

    for (int x = -20; x <= 20; x += 2) {
        for (int z = -65; z <= -20; z += 2) {
            float choose_mat = rng::random_float(sampler);

            vec3 center(
                static_cast<float>(x) + 0.8f * rng::random_float(-0.5f, 0.5f, sampler),
                0.55f,
                static_cast<float>(z) + 0.8f * rng::random_float(-0.5f, 0.5f, sampler)
            );

            if ((center - vec3(0.0f, 0.5f, -40.0f)).length() > 4.5f &&
                (center - vec3(-7.0f, 0.5f, -45.0f)).length() > 4.5f &&
                (center - vec3(7.0f, 0.5f, -45.0f)).length() > 4.5f) {

                std::shared_ptr<material> sphere_mat;

                if (choose_mat < 0.30f) {
                    color albedo = rng::random_vec3(sampler) * rng::random_vec3(sampler);
                    sphere_mat = std::make_shared<lambertian>(albedo);
                } else if (choose_mat < 0.55f) {
                    color albedo = rng::random_vec3(0.5f, 1.0f, sampler);
                    sphere_mat = std::make_shared<metal>(albedo);
                } else {
                    color absorb = rng::random_vec3(0.0f, 1.0f, sampler) * 4.0f;
                    sphere_mat = std::make_shared<dielectric>(absorb, 0.0f, 1.5f);
                }

                scene.add_hittable(std::make_shared<sphere>(0.55f, center, sphere_mat));
            }
        }
    }

    scene.add_hittable(std::make_shared<sphere>(
        30.0f, vec3(5.0f, 50.0f, -45.0f), std::make_shared<light_source>(color(1.0f, 1.0f, 1.0f))
    ));

    vec3 cam_origin(0.0f, 5.0f, -8.0f);
    vec3 look_at(0.0f, 2.5f, -42.0f);
    vec3 cam_forward = unit_vector(look_at - cam_origin);
    vec3 cam_up(0.0f, 1.0f, 0.0f);

    constexpr int width = 3840 / 5;
    constexpr int height = 2160 / 5;
    camera cam(cam_origin, cam_forward, cam_up, 45.0f, width, height);

    scene.mark_done();
    std::cout << "Done constructing BVH for Stress Test Scene" << std::endl;

    auto img = cam.capture_image(scene);
    save_rendered_image(output_filename, img);
}

// -----------------------------------------------------------------------------
// Entry Point
// -----------------------------------------------------------------------------
int main() {
    // render_dragon_mesh_scene();
    // render_sphere_and_box_scene();
    // render_triangle_scene();
    render_bvh_stress_test_scene();

    return 0;
}
