#include <string>

#include "shrink_wrap/shrink_wrap.h"

int main(int argc, char **argv) {
  std::string input = argv[1];
  std::string output = argv[2];
  double alpha = std::stod(argv[3]);
  double offset = std::stod(argv[4]);

  shrink_wrap::ShrinkWrap(input, output, alpha, offset);

  return 0;
}
