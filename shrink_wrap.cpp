#include <chrono>
#include <filesystem>
#include <getopt.h>
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

std::string generate_output_name(
    const std::string input_name,
    const double alpha,
    const double offset,
    const bool relative,
    const double ratio,
    const std::string policy
) {
    std::filesystem::path path = std::filesystem::path(input_name);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();    
    std::string output_name = stem
        + "_" + std::to_string(alpha)
        + "_" + std::to_string(offset)
        + (relative ? "_relative" : "")
        + "_" + std::to_string(ratio)
        + policy 
        + extension;    
    return output_name;
}

int main(int argc, char** argv) {
    const std::string filename = argv[1];
    const double alpha = std::stod(argv[2]);
    const double offset = std::stod(argv[3]);
    bool relative = false;
    double ratio = -1.0;
    std::string policy = "";

    static struct option long_options[] = {
        {"relative", no_argument, 0, 0},
        {"ratio", required_argument, 0, 'r'},
        {"policy", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };
    optind = 4;
    int option_index = 0;
    for (int i = 0; i < 1000; ++i) {
        int opt = getopt_long(argc, argv, "r:p:", long_options, &option_index);
        if (opt == -1) {
            break;
        }
        switch (opt) {
            case 0:
                if (long_options[option_index].name == "relative") {
                    relative = true;
                }
                break;
            case 'r':
                ratio = std::stod(optarg);
                break;
            case 'p':
                policy = optarg;
                break;
            default:
                std::cerr
                << "?? getopt returned character code " << opt << " ??"
                << std::endl;
        }
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
    shrink_wrap::shrink_wrap(points, faces, alpha, offset, wrap, relative);
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "ShrinkWrap: "
        << wrap.number_of_vertices() << " vertices, "
        << wrap.number_of_edges() << "edges, "
        << wrap.number_of_faces() << " faces"
        << std::endl;
    std::cout << "Took: " << duration.count() << " s" << std::endl;

    const bool simp = ratio > 0.0 && ratio <= 1.0;
    if (simp) {
        start = std::chrono::steady_clock::now();
        int removed = simplify::simplify(wrap, ratio, policy);
        end = std::chrono::steady_clock::now();
        duration = end - start;

        std::cout << "ShrinkWrap: "
            << wrap.number_of_vertices() << " vertices, "
            << wrap.number_of_edges() << "edges, "
            << wrap.number_of_faces() << " faces"
            << std::endl;
        std::cout << "Took: " << duration.count() << " s" << std::endl;
    }
    
    const std::string output_name =
        generate_output_name(filename, alpha, offset, relative, ratio, policy);
    std::cout << "Writing to " << output_name << std::endl;
    CGAL::IO::write_polygon_mesh(
        output_name, wrap, CGAL::parameters::stream_precision(17));
    
    return EXIT_SUCCESS;
}