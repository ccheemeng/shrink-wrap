#include "io.h"

#include <filesystem>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <tuple>

#include "mesh.h"
#include "utils.h"

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
    !std::filesystem::is_regular_file(obj_file) | obj_file.extension() != ".obj"
  ) {
    return std::tuple<
      std::vector<std::array<double, 3>>,
      std::vector<std::pair<std::vector<size_t>, std::string>>,
      std::set<std::filesystem::path>
    >(vertices, material_face_map, material_files);
  }

  std::ifstream file_stream = std::ifstream(obj_file);
  std::string material_name = "";
  std::string line;
  while (std::getline(file_stream, line)) {
    line.erase(std::remove(line.begin(), line.end(), '\n'));
    line.erase(std::remove(line.begin(), line.end(), '\r'));
    std::vector<std::string> parts = utils::Split(line, ' ')

    if (parts.empty()) {
      continue;
    }

    if (parts[0] == "v") {
      if (parts.size() < 4) {
        continue;
      }
      std::optional<std::array<double, 3>> vertex_optional
        = utils::StringsToArrayFloat3(parts[1], parts[2], parts[3]);
      vertex_optional.transform(
        [vertices](const std::array<double, 3>& vertex) {
          return vertices.push_back(vertex);
        }
      )
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
        material_faces_map.insert(
          {material_name, std::vector<std::vector<size_t>>()}
        );
      }
      material_faces_map[material_name].push_back(face);
    } else if (parts[0] == "usemtl") {
      if (parts.size() < 2) {
        material_name = "";
        continue;
      }
      std::vector<std::string> material_name_parts = utils::Split(parts, ' ');
      material_name = std::accumulate(
        material_name_parts.cbegin() + 1,
        material_name_parts.cend(),
        "",
        [](std::string first, std::string second) {
          return first + " " + second;
        }
      )
    } else if (parts[0] == "mtllib") {
      if (parts.size() < 2) {
        continue;
      }
      std::vector<std::string> material_file_parts = utils::Split(parts, ' ');
      material_files = std::accumulate(
        material_file_parts.cbegin() + 1,
        material_file_parts.cend(),
        "",
        [](std::string first, std::string second) {
          return first + " " + second;
        }
      )
    }
  }
  file_stream.close();

  return std::tuple<
    std::vector<std::array<double, 3>>,
    std::vector<std::pair<std::vector<size_t>, std::string>>,
    std::set<std::filesystem::path>
  >(vertices, material_face_map, material_files);
}
}

mesh::Mesh IoObj::ReadDirectory(const std::string& directory) {
  std::filesystem::path directory_path = std::filesystem::path(directory)
  if (
    !std::filesystem::exists(directory_path)
      | !std::filesystem::is_directory(directory_path)
  ) {
    return mesh::Mesh();
  }

  std::array<double, 3> vertices;
  for (
    const std::filesystem::directory_entry& entry
      : std::filesystem::directory_iterator(directory_path)
  ) {
    if (!entry.is_regular_file()) {
      continue;
    }
    std::string extension = entry.path().extension().string();
    if (extension == ".obj") {
      ReadObj(entry.path());
    }
  }
}

bool IoObj::IsWritable(const std::string& target) {}

bool IoObj::Write(const std::string& target) {
  if (!IoObj::IsWritable(target)) {
    return false;
  }
}
}  // namespace io
