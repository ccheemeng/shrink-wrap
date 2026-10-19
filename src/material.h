#ifndef SHRINK_WRAP_MATERIAL_H_
#define SHRINK_WRAP_MATERIAL_H_

#include <array>
#include <string>
#include <optional>

namespace shrink_wrap {
namespace mesh {
namespace internal {
struct Dissolve {
  bool halo;
  double dissolve;
};
}  // namespace internal

class Material {
  public:
    static Material FromString(const std::string& description);
    std::string ToString() const;

    std::string GetName() const;

  private:
    std::string name_;

    std::optional<std::array<double, 3>> ambient_;
    std::optional<std::array<double, 3>> diffuse_;
    std::optional<std::array<double, 3>> specular_;
    std::optional<std::array<double, 3>> transmission_;
    std::optional<int> illumination_;
    std::optional<internal::Dissolve> dissolve_;
    std::optional<double> specular_exponent_;
    std::optional<double> sharpness_;
    std::optional<double> optical_density_;
};
}  // namespace mesh
}  // namespace shrink_wrap

#endif  // SHRINK_WRAP_MATERIAL_H_
