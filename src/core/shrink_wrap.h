#ifndef SHRINK_WRAP_CORE_SHRINK_WRAP_H_
#define SHRINK_WRAP_CORE_SHRINK_WRAP_H_

#include <vector>

#include "core/mesh.h"

namespace shrink_wrap {
mesh::Mesh ShrinkWrap(
  const mesh::Mesh& mesh, const double& alpha, const double& offset
);
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_CORE_SHRINK_WRAP_H_
