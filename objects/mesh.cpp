#include "mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "triangle.h"
#include "../geometry/bvh.h"
#include "../geometry/collision_info.h"

struct VertexDataIndices {
    int v_idx = 0;
    int vn_idx = 0;
};

static inline int resolve_obj_index(int idx, size_t total_count) {
    if (idx > 0) return idx - 1;
    if (idx < 0) return static_cast<int>(total_count) + idx;
    return -1;
}

mesh::mesh(const std::string& obj_file, const std::shared_ptr<material> &mat) {
    this->mat = mat;
    this->center = vec3(0.0f, 0.0f, 0.0f);

    std::ifstream file(obj_file);
    if (!file.is_open()) {
        std::cerr << "Error opening OBJ file: " << obj_file << std::endl;
        return;
    }

    float total_sa = 0.0f;
    std::vector<vec3> temp_poses;
    std::vector<vec3> temp_normals;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            vec3 p;
            ss >> p.e[0] >> p.e[1] >> p.e[2];
            temp_poses.push_back(p);
        } else if (prefix == "vn") {
            vec3 n;
            ss >> n.e[0] >> n.e[1] >> n.e[2];
            temp_normals.push_back(unit_vector(n));
        } else if (prefix == "f") {
            std::vector<VertexDataIndices> face_vertices;
            std::string vertex_str;

            while (ss >> vertex_str) {
                VertexDataIndices vertex_data;
                int v = 0, vt = 0, vn = 0;

                // Handles v, v/vt, v//vn, and v/vt/vn formats cleanly
                if (sscanf(vertex_str.c_str(), "%d/%d/%d", &v, &vt, &vn) == 3
                    || sscanf(vertex_str.c_str(), "%d//%d", &v, &vn) == 2) {
                    vertex_data.v_idx = v;
                    vertex_data.vn_idx = vn;
                } else if (sscanf(vertex_str.c_str(), "%d/%d", &v, &vt) == 2
                        || sscanf(vertex_str.c_str(), "%d", &v) == 1) {
                    vertex_data.v_idx = v;
                }

                face_vertices.push_back(vertex_data);
            }

            // Fan triangulation for convex n-gons
            for (size_t i = 1; i + 1 < face_vertices.size(); ++i) {
                int i1 = resolve_obj_index(face_vertices[0].v_idx, temp_poses.size());
                int i2 = resolve_obj_index(face_vertices[i].v_idx, temp_poses.size());
                int i3 = resolve_obj_index(face_vertices[i + 1].v_idx, temp_poses.size());

                if (i1 < 0 || i2 < 0 || i3 < 0) continue;

                vec3 p1 = temp_poses[i1];
                vec3 p2 = temp_poses[i2];
                vec3 p3 = temp_poses[i3];

                int ni1 = resolve_obj_index(face_vertices[0].vn_idx, temp_normals.size());
                int ni2 = resolve_obj_index(face_vertices[i].vn_idx, temp_normals.size());
                int ni3 = resolve_obj_index(face_vertices[i + 1].vn_idx, temp_normals.size());

                vec3 n1, n2, n3;
                if (ni1 >= 0 && ni2 >= 0 && ni3 >= 0) {
                    n1 = temp_normals[ni1];
                    n2 = temp_normals[ni2];
                    n3 = temp_normals[ni3];
                } else {
                    vec3 geom_normal = unit_vector(cross(p2 - p1, p3 - p1));
                    n1 = n2 = n3 = geom_normal;
                }

                auto tri = std::make_shared<triangle>(p1, p2, p3, n1, n2, n3, mat);
                float sa = tri->surface_area();

                center += tri->centroid() * sa;
                total_sa += sa;
                triangles.push_back(tri);
            }
        }
    }
    file.close();

    if (total_sa > 1e-8f) {
        center *= (1.0f / total_sa);
    }

    bounding_volume_hierarchy = make_bvh(triangles);
}

bool mesh::hit(ray& r, collision_info& hit_info) const {
    bool ret = hit_bvh(triangles, bounding_volume_hierarchy, r, hit_info);
    hit_info.has_volume = true;
    return ret;
}

aabb mesh::get_bounding_box() const {
    if (bounding_volume_hierarchy) {
        return bounding_volume_hierarchy->bounding_box;
    }
    return { vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f) };
}

vec3 mesh::centroid() const {
    return center;
}
