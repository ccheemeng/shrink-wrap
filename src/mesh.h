#ifndef SHRINK_WRAP_MESH_H_
#define SHRINK_WRAP_MESH_H_

#include <array>
#include <optional>
#include <string>

#include "face.h"
#include "material.h"

namespace shrink_wrap {
namespace mesh {
/* Wraps a mesh defined by vertices, faces, and optional materials.

Faces are defined by index references to the vertices and materials.
*/
class Mesh {
  public:
    Mesh();
    Mesh(std::vector<std::array<double, 3>> vertices, std::vector<Face> faces);
    Mesh(
      std::vector<std::array<double, 3>> vertices,
      std::vector<Face> faces,
      std::vector<Material> materials
    );

    std::vector<std::array<double, 3>> GetVertices() const;
    std::vector<Face> GetFaces() const;
    std::vector<Material> GetMaterials() const;

    /* Create a Mesh from the materials of `this`. */
    Mesh SetVerticesFaces(
      const std::vector<std::array<double, 3>>& vertices,
      const std::vector<Face>& faces
    ) const;

    /* Returns a string representation of `this`. */
    std::string ToString() const;

    /* Returns a triangle Mesh by triangulating `this`. */
    Mesh Triangulate() const;

  private:
    std::vector<std::array<double, 3>> vertices_;
    std::vector<Face> faces_;
    std::vector<Material> materials_;
};
}  // namespace mesh
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_MESH_H_
