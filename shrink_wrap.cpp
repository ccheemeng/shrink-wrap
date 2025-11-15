#include <filesystem>
#include <getopt.h>

#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Delaunay_triangulation_cell_base_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/Polygon_mesh_processing/remesh_planar_patches.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

#include "shrink_wrap.h"
#include "simplify.h"

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = K::Point_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;
using Delaunay = CGAL::Delaunay_triangulation_3<
    K,
    CGAL::Triangulation_data_structure_3<
        CGAL::Triangulation_vertex_base_with_info_3<size_t, K>,
        CGAL::Delaunay_triangulation_cell_base_3<K>>,
    CGAL::Fast_location>;

std::string generate_output_name(const std::string input_name,
                                 const double alpha, const double offset,
                                 const bool relative, const bool simp,
                                 const double ratio, const std::string policy,
                                 const bool remesh, const double max_angle) {
    std::filesystem::path path = std::filesystem::path(input_name);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();
    std::string output_name =
        stem + "_" + std::to_string(alpha) + "_" + std::to_string(offset) +
        (relative ? "_relative" : "") +
        (simp ? "_simplify" + std::to_string(ratio) + policy : "") +
        (remesh ? "_remesh" + std::to_string(max_angle) : "") + extension;
    return output_name;
}

int main(int argc, char **argv) {
    // Input
    const double alpha = std::stod(argv[1]);
    const double offset = std::stod(argv[2]);
    std::vector<std::string> filenames;
    bool relative = false;
    bool simp = false;
    double ratio = -1.0;
    std::string policy = "";
    bool remesh = false;
    double max_angle = 0.0;
    std::string out = "";

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"relative", no_argument, 0, 0},
        {"simplify", required_argument, 0, 's'},
        {"policy", required_argument, 0, 'p'},
        {"remesh", optional_argument, 0, 'r'},
        {"out", required_argument, 0, 'o'},
        {0, 0, 0, 0}};
    optind = 3;
    int option_index = 0;
    for (int i = 0; i < 1000; ++i) {
        int opt =
            getopt_long(argc, argv, "i:s:p:r::o:", long_options, &option_index);
        if (opt == -1) {
            break;
        }
        switch (opt) {
        case 0: {
            if (long_options[option_index].name == "relative") {
                relative = true;
            }
            break;
        }
        case 'i': {
            filenames.push_back(optarg);
            break;
        }
        case 's': {
            ratio = std::stod(optarg);
            simp = ratio > 0.0 && ratio <= 1.0;
            break;
        }
        case 'p': {
            policy = optarg;
            break;
        }
        case 'r': {
            remesh = true;
            if (optarg) {
                max_angle = std::stod(optarg);
            }
            break;
        }
        case 'o': {
            out = optarg;
            break;
        }
        default: {
            std::cerr << "?? getopt returned character code " << opt << " ??"
                      << std::endl;
        }
        }
    }
    if (filenames.size() <= 0) {
        std::cerr << "?? No input files ??" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<Point_3> points;
    std::vector<std::vector<std::size_t>> faces;
    for (std::string filename : filenames) {
        std::vector<Point_3> file_points;
        std::vector<std::vector<std::size_t>> file_faces;
        std::cout << "Reading " << filename << "..." << std::endl;
        if (!CGAL::IO::read_polygon_soup(filename, file_points, file_faces) ||
            file_faces.empty()) {
            std::cout << "Invalid input: " << filename << std::endl;
            continue;
        }
        std::size_t num_points = points.size();
        for (const std::vector<std::size_t> &face : file_faces) {
            std::vector<std::size_t> new_face;
            new_face.reserve(face.size());
            for (std::size_t i : face) {
                new_face.push_back(i + num_points);
            }
            faces.push_back(new_face);
        }
        points.insert(points.end(),
                      std::make_move_iterator(file_points.begin()),
                      std::make_move_iterator(file_points.end()));
        std::cout << filename << ": " << file_points.size() << " points, "
                  << file_faces.size() << " faces" << std::endl;
    }

    if (points.empty() && faces.empty()) {
        std::cerr << "?? Could not read any points or faces from input files ??"
                  << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Input: " << points.size() << " points, " << faces.size()
              << " faces" << std::endl;

    // Input preprocessing
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    std::cout << "Triangulating input faces..." << std::endl;

    std::vector<std::vector<size_t>> triangles;
    for (std::vector<size_t> face : faces) {
        if (face.size() == 3) {
            triangles.push_back(face);
        } else if (face.size() > 3) {
            std::vector<std::pair<Point_3, size_t>> face_points;
            face_points.reserve(face.size());
            for (size_t i : face) {
                face_points.push_back(std::pair<Point_3, size_t>(points[i], i));
            }
            Delaunay delaunay =
                Delaunay(face_points.begin(), face_points.end());
            for (Delaunay::Facet facet : delaunay.finite_facets()) {
                std::vector<size_t> triangle;
                triangle.reserve(3);
                for (int j = 0; j < 4; ++j) {
                    if (j == facet.second) {
                        continue;
                    }
                    triangle.push_back(facet.first->vertex(j)->info());
                }
                triangles.push_back(triangle);
            }
        }
    }

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Triangulate: " << triangles.size() << " triangles"
              << std::endl;

    // ShrinkWrap
    start = std::chrono::steady_clock::now();
    std::cout << "ShrinkWrapping..." << std::endl;

    Surface_mesh wrap;
    shrink_wrap::shrink_wrap(points, triangles, alpha, offset, wrap, relative);

    end = std::chrono::steady_clock::now();
    duration = end - start;
    std::cout << "ShrinkWrap: " << wrap.number_of_vertices() << " vertices, "
              << wrap.number_of_edges() << " edges, " << wrap.number_of_faces()
              << " faces" << std::endl;
    std::cout << "Took: " << duration.count() << " s" << std::endl;

    // Simplify
    if (simp) {
        std::cout << "Simplifying by " << std::to_string(ratio) << "..."
                  << std::endl;
        start = std::chrono::steady_clock::now();

        int removed = simplify::simplify(wrap, ratio, policy);

        end = std::chrono::steady_clock::now();
        duration = end - start;
        std::cout << "Simplify: " << wrap.number_of_vertices() << " vertices, "
                  << wrap.number_of_edges() << " edges, "
                  << wrap.number_of_faces() << " faces" << std::endl;
        std::cout << "Took: " << duration.count() << " s" << std::endl;
    }

    // Remesh
    if (remesh) {
        std::cout << "Remeshing planar patches within "
                  << std::to_string(max_angle) << "..." << std::endl;
        start = std::chrono::steady_clock::now();

        Surface_mesh remeshed;
        CGAL::Polygon_mesh_processing::remesh_planar_patches(
            wrap, remeshed,
            CGAL::parameters::cosine_of_maximum_angle(std::cos(max_angle)));
        wrap = remeshed;

        end = std::chrono::steady_clock::now();
        duration = end - start;
        std::cout << "Remesh: " << wrap.number_of_vertices() << " vertices, "
                  << wrap.number_of_edges() << " edges, "
                  << wrap.number_of_faces() << " faces" << std::endl;
        std::cout << "Took: " << duration.count() << " s" << std::endl;
    }

    // Ouptut
    if (out.empty()) {
        out = generate_output_name(filenames.front(), alpha, offset, relative,
                                   simp, ratio, policy, remesh, max_angle);
    }
    std::cout << "Writing to " << out << "..." << std::endl;
    CGAL::IO::write_polygon_mesh(out, wrap,
                                 CGAL::parameters::stream_precision(17));

    return EXIT_SUCCESS;
}
