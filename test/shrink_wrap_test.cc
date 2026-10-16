#include "core/shrink_wrap.h"

#include <vector>
#include <iostream>

int main() {
    std::cout << shrink_wrap::shrink_wrap(mesh::Mesh(std::vector<std::array<double, 3>>(), std::vector<mesh::Face>())).valid() ? "true" : "false";
    return 0;
}
