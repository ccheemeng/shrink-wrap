#ifndef SHRINK_WRAP_SHRINK_WRAP_H_
#define SHRINK_WRAP_SHRINK_WRAP_H_

#include <string>

namespace shrink_wrap {
/* Provides a public API to shrink wrap OBJ meshes.

Example:
  ShrinkWrapper::ShrinkWrap(
    "data/input/mit_teapot/",
    "data/output/mit_teapot/teapot",
    0.5,
    0.01
  );
*/
class ShrinkWrapper {
  public:
    /* Shrink wraps all OBJ meshes in the directory `input`.

    This function is not inherently memory-safe and relies on CGAL's internal
    CGAL::alpha_wrap_3 implementation.

    Args:
      input: Directory that contains OBJ meshes to be shrink wrapped.
      output: Target name of the OBJ and MTL shrink wrap files. For example, to
        write to "data/output/mit_teapot/teapot.obj" and
        "data/output/mit_teapot/teapot.mtl", use
        "data/output/mit_teapot/teapot".
      alpha: Alpha parameter. `alpha` must be greater than 0.0. See
        https://doc.cgal.org/latest/Alpha_wrap_3/index.html#title5.
      offset: Offset parameter. `offset` must be greater than 0.0. See
        https://doc.cgal.org/latest/Alpha_wrap_3/index.html#title5.

    Returns:
      A bool indicating if the process succeeded.
    */
    static bool ShrinkWrap(
      const std::string& input,
      const std::string& output,
      const double& alpha,
      const double& offset
    );
};
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_SHRINK_WRAP_H_
