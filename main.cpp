#include <iostream>
#include <memory>
#include <fstream>

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/output_image.h"
#include "objects/sphere.h"

int main() {
    camera cam(vec3(0.0, 5.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), 40.0, 1280, 720);
    hittable_list scene;
    scene.add_hittable(std::make_shared<sphere>(5.0, vec3(0.0, 5.0, -40.0), std::make_shared<lambertian>(color(.21, .0, .25))));
    scene.add_hittable(std::make_shared<sphere>(5.0, vec3(10.0, 5.0, -45.0), std::make_shared<metal>(color(.8, .8, .8))));
    scene.add_hittable(std::make_shared<sphere>(2.0, vec3(-8.0, 10.0, -45.0), std::make_shared<metal>(color(.2, .2, .2))));
    scene.add_hittable(std::make_shared<sphere>(3.0, vec3(-13.0, 7.0, -40.0), std::make_shared<metal>(color(.4, .4, .2))));
    scene.add_hittable(std::make_shared<sphere>(10000.0, vec3(0.0, -10000.0, -20.0), std::make_shared<lambertian>(color(0.8, 0.8, 0.0))));
    scene.add_hittable(std::make_shared<sphere>(1e5, vec3(), std::make_shared<light_source>()));
    auto img = cam.capture_image(scene);

    std::ofstream fout("/home/akhil/raytracing/image.ppm");
    write_image(fout, img);
    fout.close();
}
