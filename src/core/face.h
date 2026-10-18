#ifndef SHRINK_WRAP_CORE_FACE_H_
#define SHRINK_WRAP_CORE_FACE_H_

#include <optional>
#include <utility>
#include <vector>

namespace mesh {
class Face {
  public:
    static std::vector<Face> ClosestFaces(
      const std::vector<Face>& source,
      const std::vector<Face>& target,
      const std::vector<std::array<double, 3>>& vertices
    );

    Face(std::vector<std::size_t> vertices);
    Face(std::vector<std::size_t> vertices, int material);

    std::vector<std::size_t> GetVertices() const;
    std::optional<int> GetMaterial() const;
    std::optional<std::pair<std::size_t, std::size_t>> GetVertexMinmax() const;
    Face SetVertices(const std::vector<std::size_t>& vertices) const;
    Face SetMaterial(int material) const;

    bool Valid() const;
    int NumVertices() const;
    std::array<double, 3> Barycentre(
      const std::vector<std::array<double, 3>>& vertices
    ) const;
    std::vector<Face> Triangulate(
      const std::vector<std::array<double, 3>>& vertices
    ) const;

  private:
    Face(
      std::vector<std::size_t> vertices,
      std::optional<int> material,
      std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax
    );

    std::vector<std::size_t> vertices_;
    std::optional<int> material_;
    std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax_;
};
}

#endif  // SHIRNK_WRAP_CORE_FACE_H_
