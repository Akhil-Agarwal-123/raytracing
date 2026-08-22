#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include <vector>
#include <memory>

#include "vec3.h"
#include "../graphics/material.h"

class ray {
public:
    vec3 orig, dir;

    ray(const vec3& origin, const vec3& direction,
        const std::vector<std::shared_ptr<material>>& prevMaterials,
        const std::shared_ptr<material>& newMaterial)
        : orig(origin), dir(direction), materials(prevMaterials) {
        materials.push_back(newMaterial);
    }

    ray(const vec3& origin, const vec3& direction,
        const std::vector<std::shared_ptr<material>>& prevMaterials = {})
        : orig(origin), dir(direction), materials(prevMaterials) {}

    [[nodiscard]] inline float n() const {
        if (materials.empty()) return 1.0f;
        return materials.back()->refractive_index;
    }

    // Direct access to previous medium without mutation
    [[nodiscard]] inline float next_n() const {
        if (materials.size() < 2) return 1.0f;
        return materials[materials.size() - 2]->refractive_index;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<material>>& get_mat_stack() const {
        return materials;
    }

    inline void remove_last_mat() {
        if (!materials.empty()) {
            materials.pop_back();
        }
    }

    inline void add_new_mat(const std::shared_ptr<material>& new_mat) {
        materials.push_back(new_mat);
    }

    [[nodiscard]] inline vec3 at(const float t) const {
        return orig + t * dir;
    }

private:
    std::vector<std::shared_ptr<material>> materials;
};

#endif // RAYTRACING_RAY_H
