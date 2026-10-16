#ifndef SHRINK_WRAP_CORE_MESH_H_
#define SHRINK_WRAP_CORE_MESH_H_

#include <array>
#include <optional>
#include <string>

#include "core/face.h"
#include "core/material.h"

namespace mesh {
class Mesh {
  public:
    Mesh(std::vector<std::array<double, 3>> vertices, std::vector<Face> faces);
    Mesh(
      std::vector<std::array<double, 3>> vertices,
      std::vector<Face> faces,
      std::vector<Material> materials
    );

    std::vector<std::array<double, 3>> get_vertices() const;
    std::vector<Face> get_faces() const;
    bool valid() const;

  private:
    std::vector<std::array<double, 3>> vertices_;
    std::vector<Face> faces_;
    std::vector<Material> materials_;
};
}  // namespace mesh

#endif  // SHRINK_WRAP_CORE_MESH_H_
