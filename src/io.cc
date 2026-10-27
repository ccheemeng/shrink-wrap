#include "io.h"

#include <filesystem>
#include <fstream>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "face.h"
#include "material.h"
#include "mesh.h"
#include "utils.h"

namespace shrink_wrap {
namespace io {
namespace {
/* Returns all material definitions in `mtl_file`. */
std::vector<std::string> ReadMtl(const std::filesystem::path& mtl_file) {
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
    std::erase(line, '\n');
    std::erase(line, '\r');
    std::vector<std::string> parts = utils::Split(line, ' ');

    if (parts.empty()) {
      continue;
    }

    if (parts[0] == "newmtl") {
      if (!(material.empty())) {
        if (material.back() == '\n') {
          material.pop_back();
        }
        materials.push_back(material);
        material = "";
      }
    } else {
      if (material.empty()) {
        continue;
      }
    }
    material += line + "\n";
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

/* Returns an OBJ format string of all faces with a particular material.

Args:
  material_face_map: Map of faces keyed on material name.
  material_name: Material name to get faces from `material_face_map` with.
*/
std::string WriteMaterialFaces(
  std::unordered_map<
    std::string, std::vector<std::vector<size_t>>
  > material_face_map,
  const std::string& material_name
) {
  std::string lines = "";
  std::vector<std::vector<size_t>> faces = material_face_map[material_name];
  for (std::vector<size_t> face : faces) {
    std::string line = "f ";
    for (size_t i : face) {
      line += std::to_string(i + 1) + " ";
    }
    line.pop_back();
    lines += line + "\n";
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

  std::set<std::string> material_files;
  for (
    const std::filesystem::directory_entry& entry
      : std::filesystem::directory_iterator(directory_path)
  ) {
    if (
      !entry.is_regular_file()
        || (entry.path().extension().string() != ".obj")
    ) {
      continue;
    }

    std::filesystem::path parent_directory = entry.path().parent_path();
    std::ifstream file_stream = std::ifstream(entry.path());
    std::string line;
    while (std::getline(file_stream, line)) {
      std::erase(line, '\n');
      std::erase(line, '\r');
      std::vector<std::string> parts = utils::Split(line, ' ');

      if (parts.empty()) {
        continue;
      }

      if (parts[0] == "mtllib") {
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
          material_files.insert((parent_directory / material_path).string());
        }
      }
    }
    file_stream.close();
  }

  int material_index = 0;
  // Index of material names to their positions in `materials`.
  std::unordered_map<std::string, int> material_names;
  std::vector<mesh::Material> materials;
  for (const std::string& material_file : material_files) {
    std::vector<std::string> material_definitions = ReadMtl(material_file);
    materials.reserve(materials.size() + material_definitions.size());
    for (const std::string& material_definition : material_definitions) {
      mesh::Material material
        = mesh::Material::FromString(material_definition);
      if (material_names.contains(material.GetName())) {
        continue;
      }
      materials.push_back(material);
      material_names[material.GetName()] = material_index;
      ++material_index;
    }
  }

  std::vector<std::array<double, 3>> vertices;
  std::vector<mesh::Face> faces;
  for (
    const std::filesystem::directory_entry& entry
      : std::filesystem::directory_iterator(directory_path)
  ) {
    if (
      !entry.is_regular_file()
        || (entry.path().extension().string() != ".obj")
    ) {
      continue;
    }

    std::string material_name = "";
    std::ifstream file_stream = std::ifstream(entry.path());
    std::string line;
    while (std::getline(file_stream, line)) {
      std::erase(line, '\n');
      std::erase(line, '\r');
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
        if (vertex_optional.has_value()) {
          vertices.push_back(vertex_optional.value());
        }
      } else if (parts[0] == "f") {
        if (parts.size() < 4) {
          continue;
        }
        std::vector<size_t> face_vertices;
        face_vertices.reserve(parts.size() - 1);
        for (int i = 1; i < parts.size(); ++i) {
          std::vector<std::string> vs = utils::Split(parts[i], '/');
          if (vs.empty()) {
            continue;
          }
          face_vertices.push_back(utils::StringToUnsignedLongLong(vs[0]) - 1);
        }
        mesh::Face face = mesh::Face(face_vertices);
        if (material_names.contains(material_name)) {
          face = face.SetMaterial(material_names[material_name]);
        }
        faces.push_back(face);
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
      }
    }
    file_stream.close();
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
  obj_stream << "mtllib " << mtl_path.filename().string() << "\n";

  for (const std::array<double, 3>& vertex : mesh.GetVertices()) {
    obj_stream << "v "
      << std::to_string(vertex[0])
      << " " << std::to_string(vertex[1])
      << " " << std::to_string(vertex[2])
      << "\n";
  }

  std::vector<mesh::Material> materials = mesh.GetMaterials();
  std::unordered_map<
    std::string, std::vector<std::vector<size_t>>
  > material_face_map;
  for (const mesh::Face& face : mesh.GetFaces()) {
    //
    std::string material_name = "";
    std::optional<int> material = face.GetMaterial();
    if (material.has_value()) {
      material_name = materials[material.value()].GetName();
    }
    //
    // TODO: Switch to below code when upgrading to C++23.
    // std::string material_name = face
    //   .GetMaterial()
    //   .transform([materials](int material) {
    //     return materials[material].GetName();
    //   })
    //   .value_or("");

    if (!material_face_map.contains(material_name)) {
      material_face_map[material_name] = std::vector<std::vector<size_t>>();
    }
    material_face_map[material_name].push_back(face.GetVertices());
  }

  if (material_face_map.contains("")) {  // put no material faces at top of file
    obj_stream << WriteMaterialFaces(material_face_map, "") << "\n";
    material_face_map.erase("");
  }
  for (const std::string& material_name : std::views::keys(material_face_map)) {
    obj_stream << "usemtl " << material_name << "\n";
    obj_stream << WriteMaterialFaces(material_face_map, material_name) << "\n";
  }

  obj_stream.close();

  std::ofstream mtl_stream = std::ofstream(mtl_path);
  for (const mesh::Material& material : mesh.GetMaterials()) {
    mtl_stream << material.ToString() << "\n";
  }
  mtl_stream.close();

  return true;
}
}  // namespace io
}  // namespace shrink_wrap
