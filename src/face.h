#ifndef SHRINK_WRAP_FACE_H_
#define SHRINK_WRAP_FACE_H_

#include <optional>
#include <utility>
#include <vector>

namespace shrink_wrap{
namespace mesh {
/* Wraps index references to an external sequence of vertices and materials.

As the class wraps index references, it follows 0-based numbering unlike some
mesh formats like OBJ. Convenience methods are provided to perform face-related
algorithms.
*/
class Face {
  public:
    /* Returns the closest Face `source` for each Face in `target`.

    Args:
      source: Faces to query.
      target: Faces to query with.
      vertices: External sequence of vertices to materialise Faces in space.
    */
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

    /* Returns a bool indicating if `this` has 3 or more vertices. */
    bool Valid() const;

    /* Returns the number of vertices `this` contains. */
    int NumVertices() const;

    /* Returns the barycentre of `this`.

    Args:
      vertices: External sequence of vertices to materialise `this` in space.
    */
    std::array<double, 3> Barycentre(
      const std::vector<std::array<double, 3>>& vertices
    ) const;

    /* Returns the Faces from triangulating `this`.

    A 3D Delaunay triangulation is performed, so beyond the base case of a Face
    with 3 vertices, the maximum number of Faces returned is n * (n - 3), where
    n is the number of vertices.

    Args:
      vertices: External sequence of vertices to materialise `this` in space.
    */
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
}  // namespace mesh
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_FACE_H_
