#include "material.h"

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "utils.h"

namespace shrink_wrap {
namespace mesh {
Material Material::FromString(const std::string& definition) {
  Material material;

  std::string new_definition = definition;
  new_definition.erase(
    std::remove(new_definition.begin(), new_definition.end(), '\r'),
    new_definition.end()
  );

  std::vector<std::string> statements = utils::Split(new_definition, '\n');
  for (const std::string& statement : statements) {
    std::vector<std::string> parts = utils::Split(statement, ' ');

    if (parts.empty()) {
      continue;
    }

    if (parts[0] == "newmtl") {
      if (parts.size() < 2) {
        break;
      }
      material.name_ = parts[1];
    } else if (parts[0] == "Ka") {
      if (parts.size() < 4) {
        break;
      }
      material.ambient_ = utils::StringsToArrayDouble3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Kd") {
      if (parts.size() < 4) {
        break;
      }
      material.diffuse_ = utils::StringsToArrayDouble3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Ks") {
      if (parts.size() < 4) {
        break;
      }
      material.specular_ = utils::StringsToArrayDouble3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Tf") {
      if (parts.size() < 4) {
        break;
      }
      material.transmission_ = utils::StringsToArrayDouble3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "illum") {
      if (parts.size() < 2) {
        break;
      }
      material.illumination_ = utils::TryCast<std::string, int>(
        utils::StringToInt, parts[1]
      );
    } else if (parts[0] == "d") {
      if (parts.size() == 2) {
        //
        std::optional<double> dissolve = utils::TryCast<std::string, double>(
          utils::StringToDouble, parts[1]
        );
        if (dissolve.has_value()) {
          material.dissolve_ = std::optional<internal::Dissolve>(
            internal::Dissolve({false, dissolve.value()})
          );
        }
        //
        // TODO: Switch to below code when upgrading to C++23.
        // material.dissolve_ = utils::TryCast<std::string, double>(
        //   utils::StringToDouble, parts[1]
        // )
        //   .transform([](double dissolve) {
        //     return internal::Dissolve({false, dissolve});
        //   });
      } else if (parts.size() > 2 && parts[1] == "-halo") {
        //
        std::optional<double> dissolve = utils::TryCast<std::string, double>(
          utils::StringToDouble, parts[2]
        );
        if (dissolve.has_value()) {
          material.dissolve_ = std::optional<internal::Dissolve>(
            internal::Dissolve({true, dissolve.value()})
          );
        }
        //
        // TODO: Switch to below code when upgrading to C++23.
        // material.dissolve_ = utils::TryCast<std::string, double>(
        //   utils::StringToDouble, parts[2]
        // )
        //   .transform([](double dissolve) {
        //     return internal::Dissolve({true, dissolve});
        //   });
      }
    } else if (parts[0] == "Ns") {
      if (parts.size() < 2) {
        break;
      }
      material.specular_exponent_ = utils::TryCast<std::string, double>(
        utils::StringToDouble, parts[1]
      );
    } else if (parts[0] == "sharpness") {
      if (parts.size() < 2) {
        break;
      }
      material.sharpness_ = utils::TryCast<std::string, double>(
        utils::StringToDouble, parts[1]
      );
    } else if (parts[0] == "Ni") {
      if (parts.size() < 2) {
        break;
      }
      material.optical_density_ = utils::TryCast<std::string, double>(
        utils::StringToDouble, parts[1]
      );
    }
  }

  return material;
}

std::string Material::ToString() const {
  //
  std::string output = "newmtl " + this->name_;
  if (this->ambient_.has_value()) {
    std::array<double, 3> rgb = this->ambient_.value();
    output += "\nKa "
      + std::to_string(rgb[0]) + " "
      + std::to_string(rgb[1]) + " "
      + std::to_string(rgb[2]);
  }
  if (this->diffuse_.has_value()) {
    std::array<double, 3> rgb = this->diffuse_.value();
    output += "\nKd "
      + std::to_string(rgb[0]) + " "
      + std::to_string(rgb[1]) + " "
      + std::to_string(rgb[2]);
  }
  if (this->specular_.has_value()) {
    std::array<double, 3> rgb = this->specular_.value();
    output += "\nKs "
      + std::to_string(rgb[0]) + " "
      + std::to_string(rgb[1]) + " "
      + std::to_string(rgb[2]);
  }
  if (this->transmission_.has_value()) {
    std::array<double, 3> rgb = this->transmission_.value();
    output += "\nTf "
      + std::to_string(rgb[0]) + " "
      + std::to_string(rgb[1]) + " "
      + std::to_string(rgb[2]);
  }
  if (this->illumination_.has_value()) {
    output += "\nillum " + std::to_string(this->illumination_.value());
  }
  if (this->dissolve_.has_value()) {
    internal::Dissolve d = this->dissolve_.value();
    output += "\nd "
      + std::string(d.halo ? "-halo " : "")
      + std::to_string(d.dissolve);
  }
  if (this->specular_exponent_.has_value()) {
    output += "\nNs " + std::to_string(this->specular_exponent_.value());
  }
  if (this->sharpness_.has_value()) {
    output += "\nsharpness " + std::to_string(this->sharpness_.value());
  }
  if (this->optical_density_.has_value()) {
    output += "\nNi " + std::to_string(this->optical_density_.value());
  }
  return output;
  //
  // TODO: Switch to below code when upgrading to C++23.
  // return "newmtl " + this->name_
  //   + this->ambient_
  //     .transform([](std::array<double, 3> rgb) {
  //       return "\nKa "
  //         + std::to_string(rgb[0]) + " "
  //         + std::to_string(rgb[1]) + " "
  //         + std::to_string(rgb[2]);
  //     })
  //     .value_or("")
  //   + this->diffuse_
  //     .transform([](std::array<double, 3> rgb) {
  //       return "\nKd "
  //         + std::to_string(rgb[0]) + " "
  //         + std::to_string(rgb[1]) + " "
  //         + std::to_string(rgb[2]);
  //     })
  //     .value_or("")
  //   + this->specular_
  //     .transform([](std::array<double, 3> rgb) {
  //       return "\nKs "
  //         + std::to_string(rgb[0]) + " "
  //         + std::to_string(rgb[1]) + " "
  //         + std::to_string(rgb[2]);
  //     })
  //     .value_or("")
  //   + this->transmission_
  //     .transform([](std::array<double, 3> rgb) {
  //       return "\nTf "
  //         + std::to_string(rgb[0]) + " "
  //         + std::to_string(rgb[1]) + " "
  //         + std::to_string(rgb[2]);
  //     })
  //     .value_or("")
  //   + this->illumination_
  //     .transform([](int x) { return "\nillum " + std::to_string(x); })
  //     .value_or("")
  //   + this->dissolve_
  //     .transform([](internal::Dissolve d) {
  //       return "\nd "
  //         + std::string(d.halo ? "-halo " : "")
  //         + std::to_string(d.dissolve);
  //     })
  //     .value_or("")
  //   + this->specular_exponent_
  //     .transform([](double x) { return "\nNs " + std::to_string(x); })
  //     .value_or("")
  //   + this->sharpness_
  //     .transform([](double x) { return "\nsharpness " + std::to_string(x); })
  //     .value_or("")
  //   + this->optical_density_
  //     .transform([](double x) { return "\nNi " + std::to_string(x); })
  //     .value_or("");
}

std::string Material::GetName() const {
  return this->name_;
}
}  // namespace mesh
}  // namespace shrink_wrap
