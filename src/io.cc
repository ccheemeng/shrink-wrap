#include "io.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <tuple>

#include "face.h"
#include "material.h"
#include "mesh.h"
#include "utils.h"

namespace shrink_wrap {
namespace io {
namespace {
std::tuple<
  std::vector<std::array<double, 3>>,
  std::map<std::string, std::vector<std::vector<size_t>>>,
  std::set<std::string>
> ReadObj(std::filesystem::path obj_file) {
  std::vector<std::array<double, 3>> vertices;
  std::map<std::string, std::vector<std::vector<size_t>>> material_face_map;
  std::set<std::string> material_files;
  if (
    !std::filesystem::is_regular_file(obj_file) || obj_file.extension() != ".obj"
  ) {
    return std::tuple<
      std::vector<std::array<double, 3>>,
      std::map<std::string, std::vector<std::vector<size_t>>>,
      std::set<std::string>
    >(vertices, material_face_map, material_files);
  }

  std::filesystem::path parent_directory = obj_file.parent_path();
  std::ifstream file_stream = std::ifstream(obj_file);
  std::string material_name = "";
  std::string line;
  while (std::getline(file_stream, line)) {
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    std::vector<std::string> parts = utils::Split(line, ' ');

    if (parts.empty()) {
      continue;
    }

    if (parts[0] == "v") {
      if (parts.size() < 4) {
        continue;
      }
      std::optional<std::array<double, 3>> vertex_optional
        = utils::StringsToArrayDouble3(parts[1], parts[2], parts[3]);
      vertex_optional.transform(
        [&vertices](const std::array<double, 3>& vertex) {
          vertices.push_back(vertex);
          return vertex;
        }
      );
    } else if (parts[0] == "f") {
      if (parts.size() < 4) {
        continue;
      }

      std::vector<size_t> face;
      face.reserve(parts.size() - 1);
      for (int i = 1; i < parts.size(); ++i) {
        std::vector<std::string> indices = utils::Split(parts[i], '/');
        if (indices.empty()) {
          continue;
        }
        face.push_back(utils::StringToUnsignedLongLong(indices[0]) - 1);
      }
      if (!material_face_map.contains(material_name)) {
        material_face_map.insert(
          {material_name, std::vector<std::vector<size_t>>()}
        );
      }
      material_face_map[material_name].push_back(face);
    } else if (parts[0] == "usemtl") {
      if (parts.size() < 2) {
        material_name = "";
        continue;
      }
      material_name = std::accumulate(
        parts.cbegin() + 2,
        parts.cend(),
        parts[1],
        [](std::string first, std::string second) {
          return first + " " + second;
        }
      );
    } else if (parts[0] == "mtllib") {
      if (parts.size() < 2) {
        continue;
      }
      std::string material_file = std::accumulate(
        parts.cbegin() + 2,
        parts.cend(),
        parts[1],
        [](std::string first, std::string second) {
          return first + " " + second;
        }
      );
      std::filesystem::path material_path = std::filesystem::path(
        material_file
      );
      if (material_path.is_absolute()) {
        material_files.insert(material_file);
      } else {
        material_files.insert(parent_directory / material_path);
      }
    }
  }
  file_stream.close();

  return std::tuple<
    std::vector<std::array<double, 3>>,
    std::map<std::string, std::vector<std::vector<size_t>>>,
    std::set<std::string>
  >(vertices, material_face_map, material_files);
}

std::vector<std::string> ReadMtl(std::filesystem::path mtl_file) {
  std::vector<std::string> materials;
  if (
    !std::filesystem::is_regular_file(mtl_file)
      || mtl_file.extension() != ".mtl"
  ) {
    return materials;
  }

  std::ifstream file_stream = std::ifstream(mtl_file);
  std::string material = "";
  std::string line;
  while (std::getline(file_stream, line)) {
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    std::vector<std::string> parts = utils::Split(line, ' ');
    if (parts.empty()) {
      continue;
    }
    if (parts[0] == "newmtl" && !material.empty()) {
        if (material.back() == '\n') {
          material.pop_back();
        }
        materials.push_back(material);
        material = "";
    }
    material += line + '\n';
  }
  file_stream.close();
  if (!material.empty()) {
    if (material.back() == '\n') {
      material.pop_back();
    }
    materials.push_back(material);
  }
  return materials;
}

std::string WriteMaterialFaces(
  std::map<std::string, std::vector<std::vector<size_t>>> material_face_map,
  std::string material_name
) {
  std::string lines = "";
  std::vector<std::vector<size_t>> faces = material_face_map[material_name];
  for (std::vector<size_t> face : faces) {
    std::string line = "f ";
    for (size_t i : face) {
      line += std::to_string(i + 1) + ' ';
    }
    line.pop_back();
    lines += line + '\n';
  }
  lines.pop_back();
  return lines;
}
}  // namespace

mesh::Mesh IoObj::ReadDirectory(const std::string& directory) {
  std::filesystem::path directory_path = std::filesystem::path(directory);
  if (
    !std::filesystem::exists(directory_path)
      || !std::filesystem::is_directory(directory_path)
  ) {
    return mesh::Mesh();
  }

  std::vector<std::array<double, 3>> vertices;
  std::vector<mesh::Face> faces;

  std::map<std::string, int> material_names;  // ordered (indexed) set
  std::set<std::string> material_files;
  int material_index = 0;

  for (
    const std::filesystem::directory_entry& entry
      : std::filesystem::directory_iterator(directory_path)
  ) {
    if (!entry.is_regular_file()) {
      continue;
    }
    std::string extension = entry.path().extension().string();
    if (extension == ".obj") {
      std::tuple<
        std::vector<std::array<double, 3>>,
        std::map<std::string, std::vector<std::vector<size_t>>>,
        std::set<std::string>
      > read_obj = ReadObj(entry.path());
      std::vector<std::array<double, 3>> obj_vertices = std::get<0>(read_obj);
      std::map<
        std::string, std::vector<std::vector<size_t>>
      > obj_material_face_map = std::get<1>(read_obj);
      std::set<std::string> obj_material_files = std::get<2>(read_obj);

      vertices.reserve(vertices.size() + obj_vertices.size());
      for (std::array<double, 3> obj_vertex : obj_vertices) {
        vertices.push_back(obj_vertex);
      }

      for (
        std::string material_name : std::views::keys(obj_material_face_map)
      ) {
        bool has_name = material_name != "";
        bool in_map = material_names.contains(material_name);
        std::vector<std::vector<size_t>> material_faces
          = obj_material_face_map[material_name];

        faces.reserve(faces.size() + material_faces.size());
        for (std::vector<size_t> material_face : material_faces) {
          mesh::Face face = mesh::Face(material_face);
          if (has_name) {  // check condition in loop for readability
            int face_material_index = material_index;
            if (in_map) {
              face_material_index = material_names[material_name];
            }
            face = face.SetMaterial(face_material_index);
          }
          faces.push_back(face);
        }

        if (has_name && !in_map) {
          material_names[material_name] = material_index;
          ++material_index;
        }
      }

      for (std::string obj_material_file : obj_material_files) {
        material_files.insert(obj_material_file);
      }
    } else if (extension == ".mtl") {
      material_files.insert(entry.path().string());
    }
  }

  std::vector<mesh::Material> materials = std::vector<mesh::Material>(
    material_names.size(), mesh::Material::FromString("")
  );
  for (std::string material_name : std::views::keys(material_names)) {
    materials[material_names[material_name]] = mesh::Material::FromString(
      "newmtl " + material_name
    );
  }
  for (std::string material_file : material_files) {
    std::filesystem::path material_path = std::filesystem::path(
      material_file
    );
    if (
      !std::filesystem::is_regular_file(material_path)
        || material_path.extension() != ".mtl"
    ) {
      continue;
    }
    std::vector<std::string> material_descriptions = ReadMtl(material_path);
    for (std::string material_description : material_descriptions) {
      mesh::Material material = mesh::Material::FromString(
        material_description
      );
      std::string material_name = material.GetName();
      if (!material_names.contains(material_name)) {
        continue;
      }
      materials[material_names[material_name]] = material;
    }
  }

  return mesh::Mesh(vertices, faces, materials);
}

bool IoObj::IsWritable(const std::string& target) {
  std::filesystem::path target_path = std::filesystem::path(target);
  return target_path.has_stem();
}

bool IoObj::Write(const mesh::Mesh& mesh, const std::string& target) {
  if (!IoObj::IsWritable(target)) {
    return false;
  }
  
  std::filesystem::path target_path = std::filesystem::path(target);
  std::filesystem::path obj_path = target_path.replace_extension(".obj");
  std::filesystem::path mtl_path = target_path.replace_extension(".mtl");
  std::filesystem::create_directory(target_path.parent_path());

  std::ofstream obj_stream = std::ofstream(obj_path);
  obj_stream << "mtllib " << mtl_path.filename().string() << '\n';

  for (std::array<double, 3> vertex : mesh.GetVertices()) {
    obj_stream << "v "
      << std::to_string(vertex[0])
      << ' ' << std::to_string(vertex[1])
      << ' ' << std::to_string(vertex[2])
      << '\n';
  }

  std::vector<mesh::Material> materials = mesh.GetMaterials();
  std::map<std::string, std::vector<std::vector<size_t>>> material_face_map;
  for (mesh::Face face : mesh.GetFaces()) {
    std::string material_name = face.GetMaterial()
      .transform([materials](int material) {
        return materials[material].GetName();
      })
      .value_or("");
    if (!material_face_map.contains(material_name)) {
      material_face_map[material_name] = std::vector<std::vector<size_t>>();
    }
    material_face_map[material_name].push_back(face.GetVertices());
  }

  if (material_face_map.contains("")) {  // put no material faces at top of file
    obj_stream << WriteMaterialFaces(material_face_map, "") << '\n';
    material_face_map.erase("");
  }
  for (std::string material_name : std::views::keys(material_face_map)) {
    obj_stream << "usemtl " << material_name << '\n';
    obj_stream << WriteMaterialFaces(material_face_map, material_name) << '\n';
  }

  obj_stream.close();

  std::ofstream mtl_stream = std::ofstream(mtl_path);
  for (mesh::Material material : mesh.GetMaterials()) {
    std::cout << material.ToString() << std::endl;
    mtl_stream << material.ToString() << '\n';
  }
  mtl_stream.close();

  return true;
}
}  // namespace io
}  // namespace shrink_wrap
