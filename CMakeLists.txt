cmake_minimum_required(VERSION 3.12...3.31)
project(shrink_wrap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

find_package(CGAL REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)
add_library(shrink_wrap_core STATIC)
target_sources(shrink_wrap_core
  PRIVATE
    src/face.cc
    src/io.cc
    src/material.cc
    src/mesh.cc
    src/shrink_wrap.cc
    src/utils.cc

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      include
    FILES
      include/shrink_wrap/shrink_wrap.h
)
target_link_libraries(shrink_wrap_core
  PRIVATE
    CGAL::CGAL
    Eigen3::Eigen
)
set_target_properties(shrink_wrap_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(shrink_wrap)
target_sources(shrink_wrap
  PRIVATE
    cli/main.cc
)
target_link_libraries(shrink_wrap
  PRIVATE
    shrink_wrap_core
)

install(
  TARGETS
    shrink_wrap
  DESTINATION
    bin
)