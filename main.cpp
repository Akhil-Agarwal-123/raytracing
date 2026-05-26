#include <iostream>
#include <memory>
#include <fstream>

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/output_image.h"
#include "objects/box.h"
#include "objects/sphere.h"

int main() {
    camera cam(vec3(0.0, 5.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), 40.0, 1280, 720);
    hittable_list scene;
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

    auto img = cam.capture_image(scene);

    std::ofstream fout("/home/akhil/raytracing/image.ppm");
    write_image(fout, img);
    fout.close();
}
