#ifndef SHRINK_WRAP_IO_H_
#define SHRINK_WRAP_IO_H_

#include <string>

#include "mesh.h"

namespace shrink_wrap {
namespace io {
/* Provides an interface to read-write OBJ files.

May be subsumed under a broader Io class if more mesh formats are required.

Example:
  IoObj::ReadDirectory("data/input/mit_teapot/");
  IoObj::Write(mesh, "data/output/mit_teapot/teapot");
*/
class IoObj {
  public:
    /* Returns one mesh::Mesh from all OBJ files in the directory `directory`.

    All valid MTL files referenced by "mtllib" statements in the OBJ files are
    parsed as materials in the mesh, so MTL files in `directory` may not be
    parsed and MTL files outside `directory` may be included.

    Material linkage between OBJ and MTl files is not preserved, so if multiple
    materials with the same name are defined ("newmtl"), the first material read
    will be parsed.

    MTL texture and reflection map statements are not parsed.

    Args:
      directory: Directory path to parse for OBJ files.
    */
    static mesh::Mesh ReadDirectory(const std::string& directory);

    /* Returns a bool indicating if `target` can be written to. */
    static bool IsWritable(const std::string& target);

    /* Writes a mesh::Mesh to `target`.

    Args:
      mesh: mesh::Mesh to write.
      target: Target name of the OBJ and MTL shrink wrap files. For example, to
        write to "data/output/mit_teapot/teapot.obj" and
        "data/output/mit_teapot/teapot.mtl", use
        "data/output/mit_teapot/teapot".

    Returns:
      A bool indicating if the write succeeded.
    */
    static bool Write(const mesh::Mesh& mesh, const std::string& target);
};
}  // namespace io
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_IO_H_
