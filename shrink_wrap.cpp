#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Real_timer.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/IO/polygon_soup_io.h>

#include "shrink_wrap.h"
#include "simplify.h"

using Point_3 = CGAL::Exact_predicates_inexact_constructions_kernel::Point_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;

namespace main{

std::string generate_output_name(
    std::string input_name,
    const double alpha,
    const double offset,
    const bool simplify
) {
    std::filesystem::path path = std::filesystem::path(input_name);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();    
    std::string output_name = stem
        + "_" + std::to_string(static_cast<int>(alpha))
        + "_" + std::to_string(static_cast<int>(offset))
        + (simplify ? "_simplified" : "")
        + extension;    
    return output_name;
}

int main(int argc, char** argv) {
    const std::string filename = argv[1];
    const double alpha = std::stod(argv[2]);
    const double offset = std::stod(argv[3]);
    bool simplify = false;
    if (argc > 4 && (argv[4] == "-s" || argv[4] == "--simplify")) {
        simplify = true;
    }

    std::cout << "Reading " << filename << "..." << std::endl;
    std::vector<Point_3> points;
    std::vector<std::vector<std::size_t>> faces;
    if (
        !CGAL::IO::read_polygon_soup(filename, points, faces)
        || faces.empty()
    ) {
        std::cerr << "Invalid input: " << filename << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Input: "
        << points.size() << " points, "
        << faces.size() << " faces"
        << std::endl;

    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    Surface_mesh wrap;
    shrink_wrap(points, faces, alpha, offset, wrap);
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "ShrinkWrap: "
        << num_vertices(wrap) << " vertices, "
        << num_faces(wrap) << " faces"
        << std::endl;
    std::cout << "Took: " << duration << " s" << std::endl;
    
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    if (simplify) {
        simplify(wrap, 0.2, "cp");
    }
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Simplify: "
        << num_vertices(wrap) << " vertices, "
        << num_faces(wrap) << " faces"
        << std::endl;
    std::cout << "Took: " << duration << " s" << std::endl;
    
    const std::string output_name =
        generate_output_name(filename, alpha, offset);
    std::cout << "Writing to " << output_name << std::endl;
    CGAL::IO::write_polygon_mesh(
        output_name, wrap, CGAL::parameters::stream_precision(17));
    
    return EXIT_SUCCESS;
}
}