#ifndef SHRINK_WRAP_SHRINK_WRAP_H_
#define SHRINK_WRAP_SHRINK_WRAP_H_

#include <string>

namespace shrink_wrap {
class ShrinkWrapper {
  public:
    static bool ShrinkWrap(
      const std::string& input,
      const std::string& output,
      const double& alpha,
      const double& offset
    );
};
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_SHRINK_WRAP_H_
