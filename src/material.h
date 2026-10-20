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

/* Wraps specifications for mesh materials.

The class is structured around the MTL format, and image maps are not supported.

May be subsumed under a broader Material class and renamed to MaterialObj if
more material formats are supported.

Example:
  Material::FromString("newmtl my_mtl\nKa 0.0435 0.0435 0.0435");
*/
class Material {
  public:
    /* Returns a Material from a material definition. */
    static Material FromString(const std::string& definition);

    /* Returns a material definition of `this`. */
    std::string ToString() const;

    /* Returns the material name of `this`. */
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
