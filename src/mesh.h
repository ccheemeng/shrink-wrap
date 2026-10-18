#ifndef SHRINK_WRAP_MESH_H_
#define SHRINK_WRAP_MESH_H_

#include <array>
#include <optional>
#include <string>

#include "face.h"
#include "material.h"

namespace mesh {
class Mesh {
  public:
    Mesh(std::vector<std::array<double, 3>> vertices, std::vector<Face> faces);
    Mesh(
      std::vector<std::array<double, 3>> vertices,
      std::vector<Face> faces,
      std::vector<Material> materials
    );

    std::vector<std::array<double, 3>> GetVertices() const;
    std::vector<Face> GetFaces() const;
    std::vector<Material> GetMaterials() const;
    Mesh SetVerticesFaces(
      const std::vector<std::array<double, 3>>& vertices,
      const std::vector<Face>& faces
    ) const;

    bool Valid() const;
    Mesh Triangulate() const;

  private:
    std::vector<std::array<double, 3>> vertices_;
    std::vector<Face> faces_;
    std::vector<Material> materials_;
};
}  // namespace mesh

#endif  // SHRINK_WRAP_MESH_H_
