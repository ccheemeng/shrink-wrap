#ifndef WRITE_OBJ_VERTICES_FACES_MATERIALS_3_H
#define WRITE_OBJ_VERTICES_FACES_MATERIALS_3_H

#include <filesystem>

#include "source/Vector_3.h"

namespace write_obj_vertices_faces_materials_3 {
bool write_obj_vertices_faces_materials_3(
    const std::string &fname, const std::vector<Vector_3<double>> &points,
    const std::vector<std::vector<size_t>> &faces,
    const std::vector<std::string> &materials,
    const std::set<std::filesystem::path> &material_files) {
    std::map<std::string, std::vector<std::vector<size_t>>> material_faces_map;
    for (int i = 0; i < std::min(faces.size(), materials.size()); ++i) {
        std::vector<size_t> face = faces[i];
        std::string material = materials[i];
        if (!material_faces_map.count(material) <= 0) {
            material_faces_map.insert(
                std::pair<std::string, std::vector<std::vector<size_t>>>(
                    material, std::vector<std::vector<size_t>>()));
        }
        material_faces_map[material].push_back(face);
    }

    std::filesystem::path fpath = std::filesystem::path(fname);
    if (!std::filesystem::exists(fpath.parent_path())) {
        std::filesystem::create_directory(fpath.parent_path());
    }
    std::ofstream obj = std::ofstream(fpath);
    std::ofstream mtl =
        std::ofstream(fpath.parent_path() /
                      std::filesystem::path(fpath.stem().string() + ".mtl"));
    obj << "mtllib " << fpath.stem().string() + ".mtl" << "\n";

    for (std::filesystem::path material_file : material_files) {
        if (!std::filesystem::exists(material_file)) {
            continue;
        }
        std::ifstream material_stream = std::ifstream(material_file);
        mtl << material_stream.rdbuf() << "\n";
        material_stream.close();
    }

    mtl.close();

    for (Vector_3<double> point : points) {
        obj << "v " << std::to_string(point.x) << " " << std::to_string(point.y)
            << " " << std::to_string(point.z) << "\n";
    }

    for (std::pair<std::string, std::vector<std::vector<size_t>>>
             material_faces : material_faces_map) {
        std::string material = material_faces.first;
        std::vector<std::vector<size_t>> faces = material_faces.second;
        obj << "usemtl " << material << "\n";
        for (std::vector<size_t> face : faces) {
            std::string line = "f ";
            for (size_t i : face) {
                line += std::to_string(i + 1) + " ";
            }
            line.pop_back();
            obj << line << "\n";
        }
    }

    obj.close();

    return true;
}
} // namespace write_obj_vertices_faces_materials_3

#endif
