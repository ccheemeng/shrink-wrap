#ifndef SHRINK_WRAP_IO_H_
#define SHRINK_WRAP_IO_H_

#include <string>

#include "mesh.h"

namespace shrink_wrap {
namespace io {
class IoObj {
  public:
    static mesh::Mesh ReadDirectory(const std::string& directory);
    static bool IsWritable(const std::string& target);
    static bool Write(const mesh::Mesh& mesh, const std::string& target);
};
}  // namespace io
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_IO_H_
