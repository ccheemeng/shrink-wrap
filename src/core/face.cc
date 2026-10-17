#include "core/face.h"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

#include "CGAL/Delaunay_triangulation_3.h"
#include "CGAL/Delaunay_triangulation_cell_base_3.h"
#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Point_3.h"
#include "CGAL/Triangulation_vertex_base_with_info_3.h"

namespace mesh {
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Delaunay = CGAL::Delaunay_triangulation_3<
  Kernel,
  CGAL::Triangulation_data_structure_3<
    CGAL::Triangulation_vertex_base_with_info_3<std::size_t, Kernel>,
    CGAL::Delaunay_triangulation_cell_base_3<Kernel>
  >,
  CGAL::Fast_location
>;
using Point_3 = Kernel::Point_3;

Face::Face(std::vector<std::size_t> vertices) {
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
  this->material_ = std::optional<int>(material);
}

Face::Face(
  std::vector<std::size_t> vertices, std::optional<int> material
) : Face(vertices) {
  this->material_ = material;
}

std::vector<std::size_t> Face::GetVertices() const {
  return this->vertices_;
}

std::optional<std::pair<std::size_t, std::size_t>> Face::GetVertexMinmax(
) const {
  return this->vertex_minmax_;
}

bool Face::Valid() const {
  return this->NumVertices() >= 3;
}

int Face::NumVertices() const {
  return this->GetVertices().size();
}

std::vector<Face> Face::Triangulate(
  const std::vector<std::array<double, 3>>& vertices
) const {
  if (!(this->Valid())) {
    return std::vector<Face>();
  }
  std::vector<Face> faces;
  std::vector<std::pair<Point_3, std::size_t>> face_points;
  for (std::size_t i : this->GetVertices()) {
    face_points.push_back(
      std::pair<Point_3, std::size_t>(
        Point_3(vertices[i][0], vertices[i][1], vertices[i][2]), i
      )
    );
  }
  Delaunay delaunay = Delaunay(face_points.begin(), face_points.end());
  for (Delaunay::Facet facet : delaunay.finite_facets()) {
    std::vector<std::size_t> triangle;
    for (int i = 0; i < 4; ++i) {
      if (i == facet.second) {
        continue;
      }
      triangle.push_back(facet.first->vertex(i)->info());
    }
    faces.push_back(Face(triangle, this->material_));
  }
  return faces;
}
}  // namespace mesh
