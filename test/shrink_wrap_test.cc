#include "core/shrink_wrap.h"

#include <vector>
#include <iostream>

int main() {
    std::cerr << (shrink_wrap::ShrinkWrap(mesh::Mesh(std::vector<std::array<double, 3>>(), std::vector<mesh::Face>()), 0.0, 0.0).Valid() ? "true" : "false") << std::endl;
    return 0;
}
