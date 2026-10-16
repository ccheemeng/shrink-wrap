#include "core/material.h"

#include <exception>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace mesh {
namespace {
  std::vector<std::string> split(
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
  std::optional<U> try_cast(
    const std::function<U(const T&)>& cast, const T& value
  ) {
    try {
      return std::optional<U>(cast(value));
    } catch (const std::exception&) {
      return std::optional<U>();
    }
  }

  std::optional<std::array<float, 3>> strings_to_array_float_3(
    const std::string& a, const std::string& b, const std::string& c
  ) {
    return try_cast<std::string, float>(std::stof, a)
      .and_then([b](float x) {
        return try_cast<std::string, float>(std::stof, b)
          .transform([x](float y) {
            return std::array<float, 2>({x, y});
          });
      })
      .and_then([c](std::array<float, 2> xy) {
        return try_cast<std::string, float>(std::stof, c)
          .transform([xy](float z) {
            return std::array<float, 3>({xy[0], xy[1], z});
          })
      });
  }
}  // namespace

Material Material::from_description(const std::string& description) {
  Material material;

  new_description = description;
  new_description
    .erase(std::remove(new_description.begin(), new_description.end(), '\r'));

  std::vector<std::string> statements = split(new_description, '\n');
  for (std::string statement : statements) {
    std::vector<std::string> parts = split(line, ' ');

    if (parts.empty()) {
      continue;
    }

    switch (parts[0]) {
      case "newmtl": {
        if (parts.size() < 2) {
          break;
        }
        material.name_ = parts[1];
        break;
      }

      case "Ka": {
        if (parts.size() < 4) {
          break;
        }
        material.ambient_ = strings_to_array_float_3(
          parts[1], parts[2], parts[3]
        );
        break;
      }

      case "Kd": {
        if (parts.size() < 4) {
          break;
        }
        material.diffuse_ = strings_to_array_float_3(
          parts[1], parts[2], parts[3]
        );
        break;
      }

      case "Ks": {
        if (parts.size() < 4) {
          break;
        }
        material.specular_ = strings_to_array_float_3(
          parts[1], parts[2], parts[3]
        );
        break;
      }

      case "Tf": {
        if (parts.size() < 4) {
          break;
        }
        material.transmission_ = strings_to_array_float_3(
          parts[1], parts[2], parts[3]
        );
        break;
      }

      case "illum": {
        if (parts.size() < 2) {
          break;
        }
        material.illumination_ = try_cast<std::string, int>(parts[1]);
        break;
      }

      case "d": {
        if (parts.size() == 2) {
          material.dissolve_ = try_cast<std::string, float>(parts[1])
            .transform([](float dissolve) {
              return internal::Dissolve({false, dissolve});
            });
        } else if (parts.size() > 2 && parts[1] == "-halo") {
          material.dissolve_ = try_cast<std::string, float>(parts[2])
            .transform([](float dissolve) {
              return internal::Dissolve({true, dissolve});
            });
        }
        break;
      }

      case "Ns": {
        if (parts.size() < 2) {
          break;
        }
        material.specular_exponent_ = try_cast<std::string, float>(parts[1]);
        break;
      }

      case "sharpness": {
        if (parts.size() < 2) {
          break;
        }
        material.sharpness_ = try_cast<std::string, float>(parts[1]);
        break;
      }

      case "Ni": {
        if (parts.size() < 2) {
          break;
        }
        material.optical_density_ = try_cast<std::string, float>(parts[1]);
      }

      default: {
        break;
      }
    }
  }

  return material;
}

std::string Material::to_description() const {
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
        return "\Kd"
          + ' ' + std::to_string(rgb[0])
          + ' ' + std::to_string(rgb[1])
          + ' ' + std::to_string(rgb[2]);
      })
      .value_or("")
    + this->specular_
      .transform([](std::array<float, 3> rgb) {
        return "\Ks"
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
      .tranform([](float x) { return "\nNi " + std::to_string(x); })
      .value_or("");
}
}  // namespace mesh
