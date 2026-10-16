#include "core/mesh.h"

#include <array>
#include <optional>
#include <utility>
#include <vector>

#include "core/face.h"
#include "core/material.h"

namespace mesh {
Mesh::Mesh(
  std::vector<std::array<double, 3>> vertices, std::vector<Face> faces
) {
  if (vertices.empty()) {
    return;
  }
  this->vertices_ = vertices;

  std::vector<Face> valid_faces;
  for (Face face : faces) {
    if (!face.valid()) {
      continue;
    }
    std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax = face
      .get_vertex_minmax()
    std::optional<bool> valid_vertices = vertex_minmax.transform(
      [vertices](std::pair<std::size_t, std::size_t> minmax) {
        return minmax.first >= 0
          && minmax.first < vertices.size()
          && minmax.second >= 0
          && minmax.second < vertices.size();
    )
    if (!valid_vertices.value_or(true)) {
      continue;
    }
    valid_faces.push_back(face);
  }
  this->faces_ = valid_faces;
}

Mesh::Mesh(
  std::vector<std::array<double, 3>> vertices,
  std::vector<Face> faces,
  std::vector<Material> materials,
) : Mesh(vertices, faces) {
  this->materials_ = materials;
}

std::vector<std::array<double, 3>> Mesh::get_vertices() const {
  return this->vertices_;
}

std::vector<Face> Mesh::get_faces() const {
  return this->faces_;
}

bool Mesh::valid() const {
  return this->get_vertices().empty();
}
}  // namespace mesh
