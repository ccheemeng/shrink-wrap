#ifndef SHRINK_WRAP_CORE_FACE_H_
#define SHRINK_WRAP_CORE_FACE_H_

#include <optional>
#include <utility>
#include <vector>

#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Point_3.h"

namespace mesh {
class Face {
  public:
    Face(std::vector<std::size_t> vertices);
    Face(std::vector<std::size_t> vertices, int material);

    std::vector<std::size_t> GetVertices() const;
    std::optional<std::pair<std::size_t, std::size_t>> GetVertexMinmax() const;

    bool Valid() const;
    int NumVertices() const;
    std::vector<Face> Triangulate(
      const std::vector<std::array<double, 3>>& vertices
    ) const;

  private:
    Face(std::vector<std::size_t> vertices, std::optional<int> material);

    std::vector<std::size_t> vertices_;
    std::optional<int> material_;
    std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax_;
};
}

#endif  // SHIRNK_WRAP_CORE_FACE_H_
