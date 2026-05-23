cmake_minimum_required(VERSION 4.2)
project(raytracing)

set(CMAKE_CXX_STANDARD 26)

add_executable(raytracing main.cpp
        util/vec3.h
        util/color.h
        util/output_image.h
)
