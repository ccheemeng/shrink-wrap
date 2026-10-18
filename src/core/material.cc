#include "core/material.h"

#include <exception>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace mesh {
namespace {
  float StringToFloat(const std::string& s) {
    return std::stof(s);
  }

  int StringToInt(const std::string& s) {
    return std::stoi(s);
  }

  std::vector<std::string> Split(
    const std::string& str, const char& delimiter
  ) {
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
    return TryCast<std::string, float>(StringToFloat, a)
      .and_then([b](float x) {
        return TryCast<std::string, float>(StringToFloat, b)
          .transform([x](float y) {
            return std::array<float, 2>({x, y});
          });
      })
      .and_then([c](std::array<float, 2> xy) {
        return TryCast<std::string, float>(StringToFloat, c)
          .transform([xy](float z) {
            return std::array<float, 3>({xy[0], xy[1], z});
          });
      });
  }
}  // namespace

Material Material::FromDescription(const std::string& description) {
  Material material;

  std::string new_description = description;
  new_description.erase(
    std::remove(new_description.begin(), new_description.end(), '\r')
  );

  std::vector<std::string> statements = Split(new_description, '\n');
  for (const std::string& statement : statements) {
    std::vector<std::string> parts = Split(statement, ' ');

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
      material.ambient_ = StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Kd") {
      if (parts.size() < 4) {
        break;
      }
      material.diffuse_ = StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Ks") {
      if (parts.size() < 4) {
        break;
      }
      material.specular_ = StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "Tf") {
      if (parts.size() < 4) {
        break;
      }
      material.transmission_ = StringsToArrayFloat3(
        parts[1], parts[2], parts[3]
      );
    } else if (parts[0] == "illum") {
      if (parts.size() < 2) {
        break;
      }
      material.illumination_ = TryCast<std::string, int>(
        StringToInt, parts[1]
      );
    } else if (parts[0] == "d") {
      if (parts.size() == 2) {
        material.dissolve_ = TryCast<std::string, float>(
          StringToFloat, parts[1]
        )
          .transform([](float dissolve) {
            return internal::Dissolve({false, dissolve});
          });
      } else if (parts.size() > 2 && parts[1] == "-halo") {
        material.dissolve_ = TryCast<std::string, float>(
          StringToFloat, parts[2]
        )
          .transform([](float dissolve) {
            return internal::Dissolve({true, dissolve});
          });
      }
    } else if (parts[0] == "Ns") {
      if (parts.size() < 2) {
        break;
      }
      material.specular_exponent_ = TryCast<std::string, float>(
        StringToFloat, parts[1]
      );
    } else if (parts[0] == "sharpness") {
      if (parts.size() < 2) {
        break;
      }
      material.sharpness_ = TryCast<std::string, float>(
        StringToFloat, parts[1]
      );
    } else if (parts[0] == "Ni") {
      if (parts.size() < 2) {
        break;
      }
      material.optical_density_ = TryCast<std::string, float>(
        StringToFloat, parts[1]
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
