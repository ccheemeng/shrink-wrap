#include "utils.h"

#include <exception>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace utils {
float StringToFloat(const std::string& s) {
  return std::stof(s);
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

template <typename T, typename U>
std::optional<U> TryCast(
  const std::function<U(const T&)>& cast, const T& value
) {
  try {
      return std::optional<U>(cast(value));
  } catch (const std::exception&) {
      return std::optional<U>();
  }
}

std::optional<std::array<float, 3>> StringsToArrayFloat3(
  const std::string& a, const std::string& b, const std::string& c
) {
  return utils::TryCast<std::string, float>(StringToFloat, a)
    .and_then([b](float x) {
      return utils::TryCast<std::string, float>(StringToFloat, b)
        .transform([x](float y) {
          return std::array<float, 2>({x, y});
        });
    })
    .and_then([c](std::array<float, 2> xy) {
      return utils::TryCast<std::string, float>(StringToFloat, c)
        .transform([xy](float z) {
          return std::array<float, 3>({xy[0], xy[1], z});
        });
    });
}
}  // namespace utils
