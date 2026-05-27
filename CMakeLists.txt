cmake_minimum_required(VERSION 4.2)
project(raytracing)

set(CMAKE_CXX_STANDARD 26)

# 1. Look for OpenMP on your system
find_package(OpenMP REQUIRED)

add_executable(raytracing main.cpp
        geometry/vec3.h
        graphics/color.h
        graphics/output_image.h
        graphics/camera.h
        objects/sphere.h
        objects/hittable.h
        geometry/ray.h
        geometry/collision_info.h
        graphics/material.h
        objects/hittable_list.h
        geometry/collision_info.cpp
        graphics/camera.cpp
        graphics/material.cpp
        objects/sphere.cpp
        util/rng.h
        objects/hittable_list.cpp
        geometry/aabb.cpp
        geometry/aabb.h
        objects/box.cpp
        objects/box.h
        geometry/bvh.h
        geometry/bvh.cpp
        objects/triangle.cpp
        objects/triangle.h
        objects/mesh.cpp
        objects/mesh.h
)
target_compile_options(raytracing PRIVATE -march=native -ffast-math)

# 2. Link OpenMP to your executable (this injects the compiler/linker flags)
target_link_libraries(raytracing PRIVATE OpenMP::OpenMP_CXX)

# 3. CRITICAL FOR RAY TRACING: Force compiler optimization flags if not explicitly set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()