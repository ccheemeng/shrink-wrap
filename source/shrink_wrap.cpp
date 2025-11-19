#include <getopt.h>

#include <CGAL/AABB_traits_3.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_triangle_primitive_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Delaunay_triangulation_cell_base_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/Polygon_mesh_processing/remesh_planar_patches.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/barycenter.h>

#include "source/Vector_3.h"
#include "source/read_obj_vertices_faces_materials_3.h"
#include "source/shrink_wrap.h"
#include "source/write_obj_vertices_faces_materials_3.h"

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = K::Point_3;
using Delaunay = CGAL::Delaunay_triangulation_3<
    K,
    CGAL::Triangulation_data_structure_3<
        CGAL::Triangulation_vertex_base_with_info_3<size_t, K>,
        CGAL::Delaunay_triangulation_cell_base_3<K>>,
    CGAL::Fast_location>;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;
using Triangle_3 = K::Triangle_3;
using Tree = CGAL::AABB_tree<
    CGAL::AABB_traits_3<K, CGAL::AABB_triangle_primitive_3<
                               K, std::vector<Triangle_3>::const_iterator>>>;

std::string generate_output_name(const std::string input_name,
                                 const double alpha, const double offset,
                                 const bool relative) {
    std::filesystem::path path = std::filesystem::path(input_name);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();
    std::string output_name = stem + "_" + std::to_string(alpha) + "_" +
                              std::to_string(offset) +
                              (relative ? "_relative" : "") + extension;
    return output_name;
}

int main(int argc, char **argv) {
    // Input
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();

    const double alpha = std::stod(argv[1]);
    const double offset = std::stod(argv[2]);
    std::vector<std::string> filenames;
    bool relative = false;
    std::string out = "";

    static struct option long_options[] = {{"input", required_argument, 0, 'i'},
                                           {"relative", no_argument, 0, 0},
                                           {"out", required_argument, 0, 'o'},
                                           {0, 0, 0, 0}};
    optind = 3;
    int option_index = 0;
    for (int i = 0; i < 1000; ++i) {
        int opt = getopt_long(argc, argv, "i:o:", long_options, &option_index);
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
    if (filenames.empty()) {
        std::cerr << "?? No input files ??" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<Vector_3<double>> vector_3s;
    std::vector<std::vector<std::size_t>> faces;
    std::vector<std::string> materials;
    std::set<std::filesystem::path> material_files;
    for (std::string filename : filenames) {
        std::vector<Vector_3<double>> file_vector_3s;
        std::vector<std::vector<std::size_t>> file_faces;
        std::vector<std::string> file_materials;
        std::set<std::filesystem::path> file_material_files;
        std::cout << "Reading " << filename << "..." << std::endl;

        if (!read_obj_vertices_faces_materials_3::
                read_obj_vertices_faces_materials_3(filename, file_vector_3s,
                                                    file_faces, file_materials,
                                                    file_material_files) ||
            file_faces.empty()) {
            std::cerr << "Invalid input: " << filename << std::endl;
            continue;
        }

        std::size_t num_vector_3s = vector_3s.size();
        for (std::vector<std::size_t> face : file_faces) {
            std::vector<std::size_t> new_face;
            new_face.reserve(face.size());
            for (std::size_t i : face) {
                new_face.push_back(i + num_vector_3s);
            }
            faces.push_back(new_face);
        }
        vector_3s.insert(vector_3s.end(), file_vector_3s.begin(),
                         file_vector_3s.end());
        materials.insert(materials.end(), file_materials.begin(),
                         file_materials.end());
        material_files.insert(file_material_files.begin(),
                              file_material_files.end());

        std::cout << filename << ": " << file_vector_3s.size() << " points, "
                  << file_faces.size() << " faces" << std::endl;
    }
    std::vector<Point_3> points;
    points.reserve(vector_3s.size());
    for (Vector_3<double> vector_3 : vector_3s) {
        points.push_back(Point_3(vector_3.x, vector_3.y, vector_3.z));
    }

    if (points.empty() && faces.empty()) {
        std::cerr << "?? Could not read any points or faces from input files ??"
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Input: " << points.size() << " points, " << faces.size()
              << " faces" << std::endl;
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl << std::endl;

    // Triangulate input
    start = std::chrono::steady_clock::now();
    std::cout << "Triangulating input faces..." << std::endl;

    std::vector<std::vector<size_t>> triangles;
    std::vector<std::string> new_materials;
    for (int i = 0; i < faces.size(); ++i) {
        std::vector<size_t> face = faces[i];
        std::string material = materials[i];
        if (face.size() == 3) {
            triangles.push_back(face);
            new_materials.push_back(material);
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
                new_materials.push_back(material);
            }
        }
    }
    materials = new_materials;

    std::cout << "Triangulate: " << triangles.size() << " triangles"
              << std::endl;
    end = std::chrono::steady_clock::now();
    duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl << std::endl;

    // ShrinkWrap
    start = std::chrono::steady_clock::now();
    std::cout << "ShrinkWrapping..." << std::endl;

    Surface_mesh wrap;
    shrink_wrap::shrink_wrap(points, triangles, alpha, offset, wrap, relative);

    std::cout << "ShrinkWrap: " << wrap.number_of_vertices() << " points, "
              << wrap.number_of_edges() << " edges, " << wrap.number_of_faces()
              << " faces" << std::endl;
    end = std::chrono::steady_clock::now();
    duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl << std::endl;

    // Preprocess input
    start = std::chrono::steady_clock::now();
    std::cout << "Building AABB tree from input..." << std::endl;

    std::vector<Triangle_3> triangle_3s;
    triangle_3s.reserve(triangles.size());
    for (std::vector<size_t> triangle : triangles) {
        triangle_3s.push_back(Triangle_3(
            points[triangle[0]], points[triangle[1]], points[triangle[2]]));
    }
    Tree tree = Tree(triangle_3s.begin(), triangle_3s.end());

    end = std::chrono::steady_clock::now();
    duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl << std::endl;

    // Query tree with barycentres of target faces
    start = std::chrono::steady_clock::now();
    std::cout << "Building face barycentres from shrink wrap..." << std::endl;

    std::vector<Point_3> barycentres;
    barycentres.reserve(wrap.number_of_faces());
    std::vector<std::vector<size_t>> out_faces;
    out_faces.reserve(wrap.number_of_faces());
    for (Surface_mesh::Face_index f : wrap.faces()) {
        std::vector<std::pair<Point_3, K::FT>> face_points;
        face_points.reserve(3);
        std::vector<size_t> out_face;
        out_face.reserve(3);
        for (Surface_mesh::Vertex_index v :
             CGAL::vertices_around_face(wrap.halfedge(f), wrap)) {
            face_points.push_back(
                std::pair<Point_3, K::FT>(wrap.point(v), 1.0));
            out_face.push_back(v);
        }
        Point_3 barycentre =
            CGAL::barycenter(face_points.begin(), face_points.end());
        barycentres.push_back(barycentre);
        out_faces.push_back(out_face);
    }

    std::cout << "Querying..." << std::endl;

    std::vector<std::string> out_materials;
    out_materials.reserve(barycentres.size());
    for (Point_3 barycentre : barycentres) {
        Tree::Point_and_primitive_id point_and_primitive_id =
            tree.closest_point_and_primitive(barycentre);
        std::size_t i = point_and_primitive_id.second - triangle_3s.cbegin();
        out_materials.push_back(materials[i]);
    }

    end = std::chrono::steady_clock::now();
    duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl;

    // Output
    std::vector<Vector_3<double>> out_points;
    out_points.reserve(wrap.number_of_vertices());
    for (Surface_mesh::Vertex_index v : wrap.vertices()) {
        Point_3 point = wrap.point(v);
        out_points.push_back(Vector_3(point.x(), point.y(), point.z()));
    }

    if (out.empty()) {
        out = generate_output_name(filenames[0], alpha, offset, relative);
    }

    std::cout << "Writing to " << out << "..." << std::endl;
    if (!write_obj_vertices_faces_materials_3::
            write_obj_vertices_faces_materials_3(
                out, out_points, out_faces, out_materials, material_files)) {
        std::cerr << "Could not write " << out << "!" << std::endl;
    }

    return EXIT_SUCCESS;
}
