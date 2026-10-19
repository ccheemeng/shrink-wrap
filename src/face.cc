#include "face.h"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

#include "CGAL/AABB_traits_3.h"
#include "CGAL/AABB_tree.h"
#include "CGAL/AABB_triangle_primitive_3.h"
#include "CGAL/barycenter.h"
#include "CGAL/Delaunay_triangulation_3.h"
#include "CGAL/Delaunay_triangulation_cell_base_3.h"
#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Point_3.h"
#include "CGAL/Triangulation_vertex_base_with_info_3.h"

namespace shrink_wrap {
namespace mesh {
namespace {
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

using Triangle_3 = Kernel::Triangle_3;
using AABB_tree = CGAL::AABB_tree<
  CGAL::AABB_traits_3<
    Kernel,
    CGAL::AABB_triangle_primitive_3<
      Kernel,
      std::vector<Triangle_3>::const_iterator>
    >
>;
}  // namespace

std::vector<Face> Face::ClosestFaces(
  const std::vector<Face>& source,
  const std::vector<Face>& target,
  const std::vector<std::array<double, 3>>& vertices
) {
  std::vector<Face> source_triangles;
  source_triangles.reserve(source.size());
  for (const Face& source_face : source) {
    for (Face triangle : source_face.Triangulate(vertices)) {
      source_triangles.push_back(triangle);
    }
  }

  std::vector<Triangle_3> triangles;
  triangles.reserve(source_triangles.size());
  for (const Face& triangle : source_triangles) {
    std::vector<Point_3> triangle_points;
    triangle_points.reserve(3);
    for (const std::size_t& i : triangle.GetVertices()) {
      triangle_points.push_back(
        Point_3(vertices[i][0], vertices[i][1], vertices[i][2])
      );
    }
    triangles.push_back(
      Triangle_3(triangle_points[0], triangle_points[1], triangle_points[2])
    );
  }

  AABB_tree aabb_tree = AABB_tree(triangles.begin(), triangles.end());

  std::vector<Face> output_faces;
  output_faces.reserve(target.size());
  for (Face target_face : target) {
    std::array<double, 3> barycentre = target_face.Barycentre(vertices);
    Point_3 barycentre_point = Point_3(
      barycentre[0], barycentre[1], barycentre[2]
    );
    AABB_tree::Point_and_primitive_id point_and_primitive_id
      = aabb_tree.closest_point_and_primitive(barycentre_point);
    std::size_t i = std::distance(
      triangles.cbegin(), point_and_primitive_id.second
    );
    output_faces.push_back(source_triangles[i]);
  }
  return output_faces;
}

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
  std::vector<std::size_t> vertices,
  std::optional<int> material,
  std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax
) {
  this->vertices_ = vertices;
  this->material_ = material;
  this->vertex_minmax_ = vertex_minmax;
}

std::vector<std::size_t> Face::GetVertices() const {
  return this->vertices_;
}

std::optional<int> Face::GetMaterial() const {
  return this->material_;
}

std::optional<std::pair<std::size_t, std::size_t>> Face::GetVertexMinmax(
) const {
  return this->vertex_minmax_;
}

Face Face::SetVertices(const std::vector<std::size_t>& vertices) const {
  return this->GetMaterial()
    .transform([vertices](int material) { return Face(vertices, material); })
    .value_or(Face(vertices));
}

Face Face::SetMaterial(int material) const {
  return Face(
    this->GetVertices(), std::optional<int>(material), this->GetVertexMinmax()
  );
}

bool Face::Valid() const {
  return this->NumVertices() >= 3;
}

int Face::NumVertices() const {
  return this->GetVertices().size();
}

std::array<double, 3> Face::Barycentre(
  const std::vector<std::array<double, 3>>& vertices
) const {
  std::vector<std::pair<Point_3, Kernel::FT>> weighted_points;
  weighted_points.reserve(this->NumVertices());
  for (const std::size_t& i : this->GetVertices()) {
    weighted_points.push_back(
      std::pair<Point_3, Kernel::FT>(
        Point_3(vertices[i][0], vertices[i][1], vertices[i][2]), Kernel::FT(1)
      )
    );
  }
  Point_3 barycentre = CGAL::barycenter(
    weighted_points.cbegin(), weighted_points.cend()
  );
  return std::array<double, 3>(
    {barycentre.x(), barycentre.y(), barycentre.z()}
  );
}

std::vector<Face> Face::Triangulate(
  const std::vector<std::array<double, 3>>& vertices
) const {
  if (!(this->Valid())) {
    return std::vector<Face>();
  }
  if (this->NumVertices() == 3) {
    return std::vector<Face>({*this});
  }

  std::vector<std::pair<Point_3, std::size_t>> face_points;
  face_points.reserve(this->NumVertices());
  for (const std::size_t& i : this->GetVertices()) {
    face_points.push_back(
      std::pair<Point_3, std::size_t>(
        Point_3(vertices[i][0], vertices[i][1], vertices[i][2]), i
      )
    );
  }
  Delaunay delaunay = Delaunay(face_points.cbegin(), face_points.cend());
  std::vector<Face> faces;
  for (const Delaunay::Facet& facet : delaunay.finite_facets()) {
    std::vector<std::size_t> triangle;
    triangle.reserve(3);
    for (int i = 0; i < 4; ++i) {
      if (i == facet.second) {
        continue;
      }
      triangle.push_back(facet.first->vertex(i)->info());
    }
    faces.push_back(this->SetVertices(triangle));
  }
  return faces;
}
}  // namespace mesh
}  // namespace shrink_wrap
