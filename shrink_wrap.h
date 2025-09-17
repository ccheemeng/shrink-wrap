#ifndef SHRINK_WRAP_H
#define SHRINK_WRAP_H

#include <string>
#include <vector>

#include <CGAL/alpha_wrap_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

namespace shrink_wrap{
    
using Point_3 = CGAL::Exact_predicates_inexact_constructions_kernel::Point_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;

void shrink_wrap(
    const std::vector<Point_3>& points,
    const std::vector<std::vector<std::size_t>>& faces,
    const double alpha,
    const double offset,
    Surface_mesh& wrap,
    bool relative = false
) {
    double new_alpha = alpha;
    double new_offset = offset;
    if (relative) {
        CGAL::Bbox_3 bbox;
        for (const Point_3& point : points) {
            bbox += point.bbox();
        }
        const double diag_length = std::sqrt(
            CGAL::square(bbox.xmax() - bbox.xmin())
            + CGAL::square(bbox.ymax() - bbox.ymin())
            + CGAL::square(bbox.zmax() - bbox.zmin())
        );
        new_alpha = diag_length / alpha;
        new_offset = diag_length / offset;
    }
    CGAL::alpha_wrap_3(points, faces, new_alpha, new_offset, wrap);
}
}

#endif