#ifndef RAYTRACING_RNG_H
#define RAYTRACING_RNG_H

#include <cstdint>
#include <cmath>

#if defined(__CUDACC__)
    #define CUDA_HOST_DEVICE __host__ __device__
#else
    #define CUDA_HOST_DEVICE
#endif

#ifndef PI_CONST
#define PI_CONST 3.14159265358979323846f
#endif

namespace rng {

    // Lightweight 8-byte state struct (fits entirely in GPU registers)
    struct Sampler {
        uint32_t state;
        uint32_t inc;

        CUDA_HOST_DEVICE Sampler() : state(0x853c49e6u), inc(0xda3e39cbu) {}

        CUDA_HOST_DEVICE explicit Sampler(uint32_t initstate, uint32_t initseq = 1u) {
            state = 0u;
            inc = (initseq << 1u) | 1u;
            next_uint();
            state += initstate;
            next_uint();
        }

        CUDA_HOST_DEVICE uint32_t next_uint() {
            uint32_t oldstate = state;
            state = oldstate * 747796405u + inc;
            uint32_t word = ((oldstate >> ((oldstate >> 28u) + 4u)) ^ oldstate) * 277803737u;
            return (word >> 22u) ^ word;
        }

        CUDA_HOST_DEVICE float uniform_float() {
            return static_cast<float>(next_uint()) * (1.0f / 4294967296.0f);
        }

        CUDA_HOST_DEVICE float uniform_float(float min, float max) {
            return min + (max - min) * uniform_float();
        }

        CUDA_HOST_DEVICE int uniform_int(int min, int max) {
            return min + static_cast<int>(next_uint() % static_cast<uint32_t>(max - min + 1));
        }
    };

    CUDA_HOST_DEVICE inline float random_float(Sampler& s) {
        return s.uniform_float();
    }

    CUDA_HOST_DEVICE inline float random_float(float min, float max, Sampler& s) {
        return s.uniform_float(min, max);
    }

    CUDA_HOST_DEVICE inline int random_int(int min, int max, Sampler& s) {
        return s.uniform_int(min, max);
    }

    CUDA_HOST_DEVICE inline vec3 random_vec3(Sampler& s) {
        return {s.uniform_float(), s.uniform_float(), s.uniform_float()};
    }

    CUDA_HOST_DEVICE inline vec3 random_vec3(float min, float max, Sampler& s) {
        return {s.uniform_float(min, max), s.uniform_float(min, max), s.uniform_float(min, max)};
    }

    CUDA_HOST_DEVICE inline vec3 random_unit_vector(Sampler& s) {
        float z = s.uniform_float(-1.0f, 1.0f);
        float a = s.uniform_float(0.0f, 2.0f * PI_CONST);
        float r = sqrtf(fmaxf(0.0f, 1.0f - z * z));
        return {r * cosf(a), r * sinf(a), z};
    }

    // Inversion-method unit sphere sampling (avoids GPU warp divergence from while-loops)
    CUDA_HOST_DEVICE inline vec3 random_in_unit_sphere(Sampler& s) {
        vec3 u = random_unit_vector(s);
        float r = cbrtf(s.uniform_float()); // Uniform radial density
        return {u.x() * r, u.y() * r, u.z() * r};
    }
} // namespace rng

#endif // RAYTRACING_RNG_H