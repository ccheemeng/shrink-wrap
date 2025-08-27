#ifndef OUTPUT_HELPER_H
#define OUTPUT_HELPER_H

#include <filesystem>
#include <string>

std::string generate_output_name(std::string input_name,
    const double alpha,
    const double offset) {
    std::filesystem::path path = std::filesystem::path(input_name);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();    
    std::string output_name = stem
        + "_" + std::to_string(static_cast<int>(alpha))
        + "_" + std::to_string(static_cast<int>(offset))
        + extension;    
    return output_name;
}
        
#endif