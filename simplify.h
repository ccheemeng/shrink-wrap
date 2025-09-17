#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include <string>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Bounded_normal_change_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/GarlandHeckbert_policies.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

namespace simplify {

namespace SMS = CGAL::Surface_mesh_simplification;
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Surface_mesh =
    CGAL::Surface_mesh<Kernel::Point_3>;
using Classic_plane =
    SMS::GarlandHeckbert_plane_policies<Surface_mesh, Kernel>;
using Prob_plane =
    SMS::GarlandHeckbert_probabilistic_plane_policies<Surface_mesh, Kernel>;
using Classic_tri =
    SMS::GarlandHeckbert_triangle_policies<Surface_mesh, Kernel>;
using Prob_tri =
    SMS::GarlandHeckbert_probabilistic_triangle_policies<Surface_mesh, Kernel>;

template <typename GHPolicies>
int collapse_gh(Surface_mesh& mesh, const double ratio) {
    SMS::Edge_count_ratio_stop_predicate<Surface_mesh> stop(ratio);

    using GH_cost = typename GHPolicies::Get_cost;
    using GH_placement = typename GHPolicies::Get_placement;
    using Bounded_GH_placement = SMS::Bounded_normal_change_placement<GH_placement>;

    GHPolicies gh_policies(mesh);
    const GH_cost& gh_cost = gh_policies.get_cost();
    const GH_placement& gh_placement = gh_policies.get_placement();
    Bounded_GH_placement placement(gh_placement);

    int removed = SMS::edge_collapse(
        mesh,
        stop,
        CGAL::parameters::get_cost(gh_cost).get_placement(placement)
    );

    return removed;
}

int simplify(
    Surface_mesh& mesh,
    const double ratio,
    const std::string& policy
) {
    int removed;
    if (policy == "ct") {
        removed = collapse_gh<Classic_tri>(mesh, ratio);
    } else if (policy == "pp") {
        removed = collapse_gh<Prob_plane>(mesh, ratio);
    } else if (policy == "pt"){
        removed = collapse_gh<Prob_tri>(mesh, ratio);
    } else {
        removed = collapse_gh<Classic_plane>(mesh, ratio);
    }
    return removed;
}
}

#endif