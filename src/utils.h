#ifndef SHRINK_WRAP_UTILS_H_
#define SHRINK_WRAP_UTILS_H_

#include <array>
#include <functional>
#include <string>
#include <optional>
#include <vector>

namespace shrink_wrap {
namespace utils {
double StringToDouble(const std::string& s);
int StringToInt(const std::string& s);
unsigned long long StringToUnsignedLongLong(const std::string& s);

std::vector<std::string> Split(const std::string& str, const char& delimiter);
std::optional<std::array<double, 3>> StringsToArrayDouble3(
  const std::string& a, const std::string& b, const std::string& c
);

template <typename T, typename U>  // allow callers to instantiate template
std::optional<U> TryCast(
  const std::function<U(const T&)>& cast, const T& value
) {
  try {
      return std::optional<U>(cast(value));
  } catch (const std::exception&) {
      return std::optional<U>();
  }
}
}  // namespace utils
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_UTILS_H_
