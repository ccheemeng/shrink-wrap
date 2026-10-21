#include "pybind11/pybind11.h"
#include "shrink_wrap/shrink_wrap.h"

PYBIND11_MODULE(shrink_wrap_python, module, pybind11::mod_gil_not_used()) {
  module.doc() = "Shrink Wrap Python";
  module.def(
    "shrink_wrap",
    &shrink_wrap::ShrinkWrap,
    "Shrink wraps all OBJ meshes in a directory."
  );
};
