#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include <stack>
#include <memory>

#include "vec3.h"
#include "../graphics/material.h"

class ray {
public:
    ray(const vec3& origin, const vec3& direction, const std::stack<std::shared_ptr<material>>& prevMaterials, const std::shared_ptr<material>& newMaterial)
        : materials_stack(prevMaterials), orig(origin), dir(direction) {
        materials_stack.push(newMaterial);
    }

    ray(const vec3& origin, const vec3& direction, const std::stack<std::shared_ptr<material>>& prevMaterials)
        : materials_stack(prevMaterials), orig(origin), dir(direction) { }

    vec3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    double n() const {
        if (materials_stack.empty()) return 1.0;
        return materials_stack.top()->refractive_index;
    }
    auto get_mat_stack() const { return materials_stack;}
    void remove_last_mat() { materials_stack.pop(); }
    void add_new_mat(const std::shared_ptr<material>& new_mat) { materials_stack.push(new_mat); }

    vec3 at(const double t) const {
        return orig + t * dir;
    }

private:
    vec3 orig, dir;
    std::stack<std::shared_ptr<material>> materials_stack;
};

#endif //RAYTRACING_RAY_H
