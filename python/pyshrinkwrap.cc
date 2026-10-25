#include "pybind11/pybind11.h"
#include "shrink_wrap/shrink_wrap.h"

#include <string>

namespace py = pybind11;

PYBIND11_MODULE(pyshrinkwrap, module, py::mod_gil_not_used()) {
  module.doc() = "Shrink Wrap Python";
  const char* docstring = "Shrink wraps all OBJ meshes in the directory "
    "input.\n\n"
    "This function is not inherently memory-safe and relies on CGAL's internal "
    "CGAL::alpha_wrap_3 implementation.\n\n"
    "Args:\n"
    "  input: Directory that contains OBJ meshes to be shrink wrapped.\n"
    "  output: Target name of the OBJ and MTL shrink wrap files. For example, "
    "to write to \"data/output/mit_teapot/teapot.obj\" and "
    "\"data/output/mit_teapot/teapot.mtl\", use "
    "\"data/output/mit_teapot/teapot\".\n"
    "  alpha: Alpha parameter. alpha must be greater than 0.0. See "
    "https://doc.cgal.org/latest/Alpha_wrap_3/index.html#title5.\n"
    "  offset: Offset parameter. offset must be greater than 0.0. See "
    "https://doc.cgal.org/latest/Alpha_wrap_3/index.html#title5.\n";
  module.def(
    "shrink_wrap",
    &shrink_wrap::ShrinkWrap,
    py::doc(docstring),
    py::arg("input").none(false),
    py::arg("output").none(false),
    py::arg("alpha").none(false),
    py::arg("offset").none(false)
  );
};
