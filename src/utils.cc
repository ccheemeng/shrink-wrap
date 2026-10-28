#include "utils.h"

#include <exception>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace shrink_wrap {
namespace utils {
double StringToDouble(const std::string& s) {
  return std::stod(s);
}

int StringToInt(const std::string& s) {
  return std::stoi(s);
}

unsigned long long StringToUnsignedLongLong(const std::string& s) {
  return std::stoull(s);
}

std::vector<std::string> Split(const std::string& str, const char& delimiter) {
  std::stringstream stream = std::stringstream(str);
  std::vector<std::string> parts;
  std::string part;
  while (std::getline(stream, part, delimiter)) {
      if (part.length() > 0) {
      parts.push_back(part);
      }
  }
  return parts;
}

std::optional<std::array<double, 3>> StringsToArrayDouble3(
  const std::string& a, const std::string& b, const std::string& c
) {
  //
  std::optional<double> x = TryCast<std::string, double>(StringToDouble, a);
  if (!x.has_value()) {
    return std::optional<std::array<double, 3>>();
  }
  std::optional<double> y = TryCast<std::string, double>(StringToDouble, b);
  if (!y.has_value()) {
    return std::optional<std::array<double, 3>>();
  }
  std::optional<double> z = TryCast<std::string, double>(StringToDouble, c);
  if (!z.has_value()) {
    return std::optional<std::array<double, 3>>();
  }
  return std::optional<std::array<double, 3>>(
    std::array<double, 3>({x.value(), y.value(), z.value()})
  );
  //
  // TODO: Switch to below code when upgrading to C++23.
  // return TryCast<std::string, double>(StringToDouble, a)
  //   .and_then([b](double x) {
  //     return TryCast<std::string, double>(StringToDouble, b)
  //       .transform([x](double y) {
  //         return std::array<double, 2>({x, y});
  //       });
  //   })
  //   .and_then([c](std::array<double, 2> xy) {
  //     return TryCast<std::string, double>(StringToDouble, c)
  //       .transform([xy](double z) {
  //         return std::array<double, 3>({xy[0], xy[1], z});
  //       });
  //   });
}
}  // namespace utils
}  // namespace shrink_wrap
