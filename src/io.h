#ifndef SHRINK_WRAP_IO_H_
#define SHRINK_WRAP_IO_H_

#include <string>

#include "mesh.h"

namespace io {
class IoObj {
  public:
    static mesh::Mesh ReadDirectory(const std::string& directory);
    static bool IsWritable(const std::string& target);
    static bool Write(const std::string& target);
}
}  // namespace io

#endif  // SHRINK_WRAP_IO_H_
