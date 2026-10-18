#include "material.h"

#include <optional>
#include <string>
#include <vector>

#include "utils.h"

namespace mesh {
Material Material::FromDescription(const std::string& description) {
  Material material;

  std::string new_description = description;
  new_description.erase(
    std::remove(new_description.begin(), new_description.end(), '\r')
  );

  std::vector<std::string> statements = utils::Split(new_description, '\n');
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
      material.ambient_ = utils::StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Kd") {
      if (parts.size() < 4) {
        break;
      }
      material.diffuse_ = utils::StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Ks") {
      if (parts.size() < 4) {
        break;
      }
      material.specular_ = utils::StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Tf") {
      if (parts.size() < 4) {
        break;
      }
      material.transmission_ = utils::StringsToArrayFloat3(
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
        material.dissolve_ = utils::TryCast<std::string, float>(
          utils::StringToFloat, parts[1]
        )
          .transform([](float dissolve) {
            return internal::Dissolve({false, dissolve});
          });
      } else if (parts.size() > 2 && parts[1] == "-halo") {
        material.dissolve_ = utils::TryCast<std::string, float>(
          utils::StringToFloat, parts[2]
        )
          .transform([](float dissolve) {
            return internal::Dissolve({true, dissolve});
          });
      }
    } else if (parts[0] == "Ns") {
      if (parts.size() < 2) {
        break;
      }
      material.specular_exponent_ = utils::TryCast<std::string, float>(
        utils::StringToFloat, parts[1]
      );
    } else if (parts[0] == "sharpness") {
      if (parts.size() < 2) {
        break;
      }
      material.sharpness_ = utils::TryCast<std::string, float>(
        utils::StringToFloat, parts[1]
      );
    } else if (parts[0] == "Ni") {
      if (parts.size() < 2) {
        break;
      }
      material.optical_density_ = utils::TryCast<std::string, float>(
        utils::StringToFloat, parts[1]
      );
    }
  }

  return material;
}

std::string Material::ToDescription() const {
  return "newmtl " + this->name_
    + this->ambient_
      .transform([](std::array<float, 3> rgb) {
        return "\nKa"
          + ' ' + std::to_string(rgb[0])
          + ' ' + std::to_string(rgb[1])
          + ' ' + std::to_string(rgb[2]);
      })
      .value_or("")
    + this->diffuse_
      .transform([](std::array<float, 3> rgb) {
        return "\nKd"
          + ' ' + std::to_string(rgb[0])
          + ' ' + std::to_string(rgb[1])
          + ' ' + std::to_string(rgb[2]);
      })
      .value_or("")
    + this->specular_
      .transform([](std::array<float, 3> rgb) {
        return "\nKs"
          + ' ' + std::to_string(rgb[0])
          + ' ' + std::to_string(rgb[1])
          + ' ' + std::to_string(rgb[2]);
      })
      .value_or("")
    + this->transmission_
      .transform([](std::array<float, 3> rgb) {
        return "\nTf"
          + ' ' + std::to_string(rgb[0])
          + ' ' + std::to_string(rgb[1])
          + ' ' + std::to_string(rgb[2]);
      })
      .value_or("")
    + this->illumination_
      .transform([](int x) { return "\nillum " + std::to_string(x); })
      .value_or("")
    + this->dissolve_
      .transform([](internal::Dissolve d) {
        return "\nd"
         + d.halo ? " -halo" : ""
         + ' ' + std::to_string(d.dissolve);
      })
      .value_or("")
    + this->specular_exponent_
      .transform([](float x) { return "\nNs " + std::to_string(x); })
      .value_or("")
    + this->sharpness_
      .transform([](float x) { return "\nsharpness " + std::to_string(x); })
      .value_or("")
    + this->optical_density_
      .transform([](float x) { return "\nNi " + std::to_string(x); })
      .value_or("");
}
}  // namespace mesh
