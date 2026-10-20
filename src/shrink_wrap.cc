#include "shrink_wrap/shrink_wrap.h"

#include <vector>

#include "CGAL/alpha_wrap_3.h"
#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Surface_mesh.h"

#include "face.h"
#include "io.h"
#include "material.h"
#include "mesh.h"

namespace shrink_wrap {
namespace {
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = Kernel::Point_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;

/* Converts <std::array<double, 3> vertices to Point_3 CGAL points. */
std::vector<Point_3> ToPoint3s(
  const std::vector<std::array<double, 3>>& points
) {
  std::vector<Point_3> output_points;
  output_points.reserve(points.size());
  for (const std::array<double, 3>& point : points) {
    output_points.push_back(Point_3(point[0], point[1], point[2]));
  }
  return output_points;
}

/* Converts mesh::Face indices to std::vector<std::size_t> indices. */
std::vector<std::vector<std::size_t>> ToBareFaces(
  const std::vector<mesh::Face>& faces
) {
  std::vector<std::vector<std::size_t>> output_faces;
  output_faces.reserve(faces.size());
  for (const mesh::Face face : faces) {
    output_faces.push_back(face.GetVertices());
  }
  return output_faces;
}

/* Executes the shrink wrap logic on the mesh::Mesh `mesh`. */
mesh::Mesh Run(
  const mesh::Mesh& mesh, const double& alpha, const double& offset
) {
  mesh::Mesh triangle_mesh = mesh.Triangulate();

  std::vector<Point_3> points = ToPoint3s(mesh.GetVertices());
  std::vector<std::vector<std::size_t>> faces = ToBareFaces(mesh.GetFaces());

  Surface_mesh wrap;
  CGAL::alpha_wrap_3(points, faces, alpha, offset, wrap);

  std::vector<std::array<double, 3>> output_vertices;
  output_vertices.reserve(wrap.number_of_vertices());
  for (Surface_mesh::Vertex_index vertex : wrap.vertices()) {
    Point_3 point = wrap.point(vertex);
    output_vertices.push_back({point.x(), point.y(), point.z()});
  }

  std::vector<mesh::Face> wrap_faces;
  wrap_faces.reserve(wrap.number_of_faces());
  for (Surface_mesh::Face_index face : wrap.faces()) {
    std::vector<std::size_t> output_face_vertices;
    output_face_vertices.reserve(3);
    for (
      Surface_mesh::Vertex_index vertex : CGAL::vertices_around_face(
        wrap.halfedge(face), wrap
      )
    ) {
      output_face_vertices.push_back(vertex);
    }
    wrap_faces.push_back(output_face_vertices);
  }
  // `closest_faces` is an ordered vector mapping each wrap's face to its
  // closest face in the source mesh.
  std::vector<mesh::Face> closest_faces = mesh::Face::ClosestFaces(
    mesh.GetFaces(), wrap_faces, mesh.GetVertices(), output_vertices
  );
  std::vector<mesh::Face> output_faces;
  output_faces.reserve(wrap_faces.size());
  for (std::size_t i = 0; i < wrap_faces.size(); ++i) {
    mesh::Face wrap_face = wrap_faces[i];
    // Functionally transform the closest face to the output face based on its
    // optional material attribute.
    mesh::Face output_face = closest_faces[i]
      .GetMaterial()
      .transform([wrap_face](int material) {
        return wrap_face.SetMaterial(material);
      })
      .value_or(wrap_face);
    output_faces.push_back(output_face);
  }

  mesh::Mesh output_mesh = mesh.SetVerticesFaces(output_vertices, output_faces);

  return output_mesh;
}
}  // namespace

bool ShrinkWrapper::ShrinkWrap(
  const std::string& input,
  const std::string& output,
  const double& alpha,
  const double& offset
) {
  if (!io::IoObj::IsWritable(output)) {
    return false;
  }
  mesh::Mesh input_mesh = io::IoObj::ReadDirectory(input);
  mesh::Mesh output_mesh = Run(input_mesh, alpha, offset);
  return io::IoObj::Write(output_mesh, output);
}
}  // namespace shrink_wrap
