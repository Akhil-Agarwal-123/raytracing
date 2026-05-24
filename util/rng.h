#ifndef RAYTRACING_RNG_H
#define RAYTRACING_RNG_H

#include <random>
#include <cmath>
#include <numbers>
#include "../geometry/vec3.h"

namespace rng {

    // A lightweight per-thread state holder to completely bypass lookup overhead
    struct ThreadRngState {
        std::mt19937 engine;
        std::uniform_real_distribution<double> dist;

        ThreadRngState() : engine(std::random_device{}()), dist(0.0, 1.0) {}
    };

    // The ONLY thread_local variable. Accessed exactly ONCE per function call.
    inline ThreadRngState& get_state() {
        static thread_local ThreadRngState state;
        return state;
    }

    inline double random_double() {
        auto& state = get_state();
        return state.dist(state.engine);
    }

    inline double random_double(double min, double max) {
        return min + (max - min) * random_double();
    }

    inline int random_int(int min, int max) {
        auto& state = get_state();
        std::uniform_int_distribution<int> int_dist(min, max);
        return int_dist(state.engine);
    }

    inline vec3 random_vec3() {
        auto& state = get_state();
        return {state.dist(state.engine), state.dist(state.engine), state.dist(state.engine)};
    }

    inline vec3 random_vec3(double min, double max) {
        return {random_double(min, max), random_double(min, max), random_double(min, max)};
    }

    inline vec3 random_in_unit_sphere() {
        while (true) {
            auto p = random_vec3(-1.0, 1.0);
            if (p.length_squared() < 1.0) return p;
        }
    }

    inline vec3 random_unit_vector() {
        double z = random_double(-1.0, 1.0);
        double a = random_double(0.0, 2.0 * std::numbers::pi);
        double r = std::sqrt(1.0 - z * z);
        return {r * std::cos(a), r * std::sin(a), z};
    }
}

#endif //RAYTRACING_RNG_H