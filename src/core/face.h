#ifndef SHRINK_WRAP_CORE_FACE_H_
#define SHRINK_WRAP_CORE_FACE_H_

#include <optional>
#include <utility>
#include <vector>

namespace mesh {
class Face {
public:
  Face(std::vector<std::size_t> vertices);
  Face(std::vector<std::size_t> vertices, int material);

  std::optional<std::pair<std::size_t, std::size_t>> get_vertex_minmax() const;
  bool valid() const;

private:
  std::vector<std::size_t> vertices_;
  std::optional<int> material_;
  std::optional<std::pair<std::size_t, std::size_t>> vertex_minmax_;
};
}

#endif  // SHIRNK_WRAP_CORE_FACE_H_
