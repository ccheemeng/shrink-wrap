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
    if (!face.Valid()) {
      continue;
    }
    std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax = face
      .GetVertexMinmax();
    std::optional<bool> valid_vertices = vertex_minmax.transform(
      [vertices](std::pair<std::size_t, std::size_t> minmax) {
        return minmax.first >= 0
          && minmax.first < vertices.size()
          && minmax.second >= 0
          && minmax.second < vertices.size();
      }
    );
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
  std::vector<Material> materials
) : Mesh(vertices, faces) {
  this->materials_ = materials;
}

std::vector<std::array<double, 3>> Mesh::GetVertices() const {
  return this->vertices_;
}

std::vector<Face> Mesh::GetFaces() const {
  return this->faces_;
}

bool Mesh::Valid() const {
  return this->GetVertices().empty();
}
}  // namespace mesh
