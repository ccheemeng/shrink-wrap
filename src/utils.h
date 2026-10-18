#ifndef SHRINK_WRAP_UTILS_H_
#define SHRINK_WRAP_UTILS_H_

namespace utils {
float StringToFloat(const std::string& s);
int StringToInt(const std::string& s);
unsigned long long StringToUnsignedLongLong(const std::string& s);

std::vector<std::string> Split(const std::string& str, const char& delimiter);
template <typename T, typename U>
std::optional<U> TryCast(
  const std::function<U(const T&)>& cast, const T& value
);
std::optional<std::array<float, 3>> StringsToArrayFloat3(
  const std::string& a, const std::string& b, const std::string& c
);
}

#endif  // SHRINK_WRAP_UTILS_H_
