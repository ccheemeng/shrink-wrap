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
  std::pair<
    std::vector<std::size_t>::iterator, std::vector<std::size_t>::iterator
  > vertex_minmax_iterator = std::minmax_element(
    vertices.begin(), vertices.end()
  );
  this->vertex_minmax_ = std::optional<std::pair<std::size_t, std::size_t>>(
    std::pair<std::size_t, std::size_t>(
      *(vertex_minmax_iterator.first), *(vertex_minmax_iterator.second)
    )
  );
}

Face::Face(std::vector<std::size_t> vertices, int material) : Face(vertices) {
  if (vertices.size() < 3) {
    return;
  }
  this->material_ = std::optional<int>(material);
}

std::optional<std::pair<std::size_t, std::size_t>> Face::GetVertexMinmax(
) const {
  return this->vertex_minmax_;
}

bool Face::Valid() const {
  return this->GetVertexMinmax().has_value();
}
}  // namespace mesh
