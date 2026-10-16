#include "core/face.h"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace mesh {
Face::Face(std::vector<std::size_t> vertices) {
  if (vertices.size() < 3) {
    return;
  }
  this->vertices_ = vertices;
  this->vertex_minmax_ = std::optional<std::pair<std::size_t, std::size_t>>(
    std::ranges::minmax(vertices)
  );
}

Face::Face(std::vector<std::size_t> vertices, int material) : Face(vertices) {
  if (vertices.size() < 3) {
    return;
  }
  this->material_ = std::optional<int>(material);
}

std::optional<std::pair<std::size_t, std::size_t>> Face::get_vertex_minmax() const {
  return this->vertex_minmax_;
}

bool Face::valid() const {
  return this->get_vertex_minmax().has_value();
}
}  // namespace mesh
