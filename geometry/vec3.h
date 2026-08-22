#ifndef RAYTRACING_VEC3_H
#define RAYTRACING_VEC3_H

#include <cmath>
#include <iostream>

#if defined(__CUDACC__)
    #define CUDA_HOST_DEVICE __host__ __device__
#else
    #define CUDA_HOST_DEVICE
#endif

class alignas(16) vec3 {
public:
    float e[4]; // 16-byte alignment

    CUDA_HOST_DEVICE vec3() : e{0.0f, 0.0f, 0.0f, 0.0f} {}
    CUDA_HOST_DEVICE vec3(const float e0, const float e1, const float e2) : e{e0, e1, e2, 0.0f} {}

    CUDA_HOST_DEVICE inline float x() const { return e[0]; }
    CUDA_HOST_DEVICE inline float y() const { return e[1]; }
    CUDA_HOST_DEVICE inline float z() const { return e[2]; }

    CUDA_HOST_DEVICE inline vec3 operator-() const { return {-e[0], -e[1], -e[2]}; }
    CUDA_HOST_DEVICE inline float operator[](const int i) const { return e[i]; }
    CUDA_HOST_DEVICE inline float& operator[](const int i) { return e[i]; }

    CUDA_HOST_DEVICE inline vec3& operator+=(const vec3& v) {
        e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2];
        return *this;
    }

    CUDA_HOST_DEVICE inline vec3& operator-=(const vec3& v) {
        e[0] -= v.e[0]; e[1] -= v.e[1]; e[2] -= v.e[2];
        return *this;
    }

    CUDA_HOST_DEVICE inline vec3& operator*=(const float t) {
        e[0] *= t; e[1] *= t; e[2] *= t;
        return *this;
    }

    CUDA_HOST_DEVICE inline vec3& operator*=(const vec3& t) {
        e[0] *= t.e[0]; e[1] *= t.e[1]; e[2] *= t.e[2];
        return *this;
    }

    CUDA_HOST_DEVICE inline vec3& operator/=(const float t) {
        float inv = 1.0f / t;
        return *this *= inv;
    }

    CUDA_HOST_DEVICE inline float length() const {
        return sqrtf(length_squared());
    }

    CUDA_HOST_DEVICE inline float length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    CUDA_HOST_DEVICE inline bool near_zero() const {
        return length_squared() < 1e-8f;
    }
};

// ==========================================
// Vector Operations
// ==========================================

#ifndef __CUDA_ARCH__
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
#endif

CUDA_HOST_DEVICE inline vec3 operator+(const vec3& u, const vec3& v) {
    return {u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]};
}

CUDA_HOST_DEVICE inline vec3 operator-(const vec3& u, const vec3& v) {
    return {u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]};
}

CUDA_HOST_DEVICE inline vec3 operator*(const vec3& u, const vec3& v) {
    return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

CUDA_HOST_DEVICE inline vec3 operator*(const float t, const vec3& v) {
    return {t * v.e[0], t * v.e[1], t * v.e[2]};
}

CUDA_HOST_DEVICE inline vec3 operator*(const vec3& v, const float t) {
    return t * v;
}

CUDA_HOST_DEVICE inline vec3 operator/(const vec3& v, const float t) {
    return 1.0f / t * v;
}

CUDA_HOST_DEVICE inline float dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

CUDA_HOST_DEVICE inline vec3 cross(const vec3& u, const vec3& v) {
    return {u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]};
}

CUDA_HOST_DEVICE inline vec3 exp(const vec3& u) {
    return {expf(u.e[0]), expf(u.e[1]), expf(u.e[2])};
}

CUDA_HOST_DEVICE inline vec3 unit_vector(const vec3& v) {
#if defined(__CUDA_ARCH__)
    float inv_len = rsqrtf(v.length_squared());
    return v * inv_len;
#else
    return v / v.length();
#endif
}

CUDA_HOST_DEVICE inline vec3 min(const vec3& u, const vec3& v) {
    return {fminf(u.e[0], v.e[0]), fminf(u.e[1], v.e[1]), fminf(u.e[2], v.e[2])};
}

CUDA_HOST_DEVICE inline vec3 max(const vec3& u, const vec3& v) {
    return {fmaxf(u.e[0], v.e[0]), fmaxf(u.e[1], v.e[1]), fmaxf(u.e[2], v.e[2])};
}

#endif // RAYTRACING_VEC3_H