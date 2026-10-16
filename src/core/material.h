#ifndef SHRINK_WRAP_CORE_MATERIAL_H_
#define SHRINK_WRAP_CORE_MATERIAL_H_

#include <array>
#include <string>
#include <optional>

namespace mesh {
namespace internal {
struct Dissolve {
  bool halo;
  float dissolve;
};
}  // namespace internal

class Material {
  public:
    static Material from_description(const std::string& description);
    std::string to_description() const;

  private:
    std::string name_;

    std::optional<std::array<float, 3>> ambient_;
    std::optional<std::array<float, 3>> diffuse_;
    std::optional<std::array<float, 3>> specular_;
    std::optional<std::array<float, 3>> transmission_;
    std::optional<int> illumination_;
    std::optional<internal::Dissolve> dissolve_;
    std::optional<float> specular_exponent_;
    std::optional<float> sharpness_;
    std::optional<float> optical_density_;
};
}  // namespace mesh

#endif  // SHRINK_WRAP_CORE_MATERIAL_H_
