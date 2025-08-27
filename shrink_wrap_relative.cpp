#include <iostream>
#include <string>
#include <vector>

#include <CGAL/alpha_wrap_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Real_timer.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/IO/polygon_soup_io.h>

#include "output_helper.h"

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = K::Point_3;

using Mesh = CGAL::Surface_mesh<Point_3>;

int main(int argc, char** argv) {
    std::cout.precision(17);

    const std::string filename = argv[1];
    const double relative_alpha = (argc > 2) ? std::stod(argv[2]) : 20.0;
    const double relative_offset = (argc > 3) ? std::stod(argv[3]) : 600.0;

    std::cout << "Reading " << filename << "..." << std::endl;
    std::vector<Point_3> points;
    std::vector<std::vector<std::size_t>> faces;
    if (!CGAL::IO::read_polygon_soup(filename, points, faces)
        || faces.empty()) {
        std::cerr << "Invalid input: " << filename << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Input: "<< points.size() << " points, " <<
        faces.size() << " faces" << std::endl;
    
    CGAL::Bbox_3 bbox;
    for (const Point_3& p : points) {
        bbox += p.bbox();
    }
    const double diag_length = std::sqrt(
        CGAL::square(bbox.xmax() - bbox.xmin()) +
        CGAL::square(bbox.ymax() - bbox.ymin()) +
        CGAL::square(bbox.zmax() - bbox.zmin())
    );
    const double alpha = diag_length / relative_alpha;
    const double offset = diag_length / relative_offset;
    std::cout << "Bbox diag length: " << diag_length << std::endl;
    std::cout << "Using alpha: " << alpha << " = " <<
        diag_length << " / " << relative_alpha << std::endl;
    std::cout << "Using offset: " << offset << " = " <<
        diag_length << " / " << relative_offset << std::endl;

    CGAL::Real_timer t;
    t.start();
    Mesh wrap;
    CGAL::alpha_wrap_3(points, faces, alpha, offset, wrap);
    t.stop();

    std::cout << "Result: " << num_vertices(wrap) << " vertices, " <<
        num_faces(wrap) << " faces" << std::endl;
    std::cout << "Took: " << t.time() << " s" << std::endl;
    const std::string output_name = generate_output_name(
        filename, relative_alpha, relative_offset);
    std::cout << "Writing to " << output_name << std::endl;
    CGAL::IO::write_polygon_mesh(
        output_name, wrap, CGAL::parameters::stream_precision(17));
    
    return EXIT_SUCCESS;
}