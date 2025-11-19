#include <iostream>
#include <string>

int main(int argc, char **argv) {
    std::string one = "./data/Tembusu L12.mtl";
    std::string two = "./data/Tembusu L12.mtl";
    bool equals = one == two;
    std::cout << (equals ? "eq" : "neq") << std::endl;
    return 0;
}
