#include "mesh.h"

#include <fstream>
#include <sstream>

#include "triangle.h"
#include "../geometry/bvh.h"
#include "../geometry/collision_info.h"

struct VertexDataIndices {
    int v_idx = -1;
    int vn_idx = -1;
};

mesh::mesh(const std::string& obj_file, const std::shared_ptr<material> &mat) {
    this->mat = mat;

    std::ifstream file(obj_file);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << obj_file << std::endl;
        return;
    }

    double total_sa = 0;

    std::vector<vec3> temp_poses, temp_normals;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        if (prefix == "v") {
            vec3 p; ss >> p.e[0] >> p.e[1] >> p.e[2];
            temp_poses.push_back(p);
        } else if (prefix == "vn") {
            vec3 n; ss >> n.e[0] >> n.e[1] >> n.e[2];
            temp_normals.push_back(n);
        } else if (prefix == "f") {
            std::vector<VertexDataIndices> face_vertices;
            std::string vertex_str;

            while (ss >> vertex_str) {
                VertexDataIndices vertex_data;
                std::stringstream vertex_ss(vertex_str);
                std::string v_part, dummy_uv, vn_part;
                std::getline(vertex_ss, v_part, '/');
                std::getline(vertex_ss, dummy_uv, '/');
                std::getline(vertex_ss, vn_part, '/');

                if (!v_part.empty()) vertex_data.v_idx = std::stoi(v_part);
                if (!vn_part.empty()) vertex_data.vn_idx = std::stoi(vn_part);

                face_vertices.push_back(vertex_data);
            }

            for (size_t i = 1; i < face_vertices.size() - 1; i++) {
                VertexDataIndices idx1 = face_vertices[0];
                VertexDataIndices idx2 = face_vertices[i];
                VertexDataIndices idx3 = face_vertices[i + 1];

                vec3 p1 = temp_poses[idx1.v_idx - 1];
                vec3 p2 = temp_poses[idx2.v_idx - 1];
                vec3 p3 = temp_poses[idx3.v_idx - 1];

                vec3 n1, n2, n3;
                if (idx1.vn_idx != -1 && idx2.vn_idx != -1 && idx3.vn_idx != -1) {
                    n1 = temp_normals[idx1.vn_idx - 1];
                    n2 = temp_normals[idx2.vn_idx - 1];
                    n3 = temp_normals[idx3.vn_idx - 1];
                } else {
                    vec3 edge1 = p2 - p1;
                    vec3 edge2 = p3 - p1;
                    vec3 geom_normal = unit_vector(cross(edge1, edge2));
                    n1 = n2 = n3 = geom_normal;
                }

                auto tri = std::make_shared<triangle>(p1, p2, p3, n1, n2, n3, mat);
                double sa = tri->surface_area();
                center += tri->centroid() * sa;
                total_sa += sa;
                triangles.push_back(tri);
            }
        }
    }
    file.close();
    center /= total_sa;

    bounding_volume_hierarchy = make_bvh(triangles);
}

bool mesh::hit(ray& r, collision_info& hit_info) const {
    auto ret = hit_bvh(triangles, bounding_volume_hierarchy, r, hit_info);
    hit_info.has_volume = true;
    return ret;
}

aabb mesh::get_bounding_box() const {
    return bounding_volume_hierarchy->bounding_box;
}

vec3 mesh::centroid() const {
    return center;
}
