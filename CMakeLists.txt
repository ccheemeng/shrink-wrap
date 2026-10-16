cmake_minimum_required(VERSION 3.12...3.29)
project(shrink_wrap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

find_package(CGAL REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
add_executable(compile_test test/shrink_wrap_test.cc ${SOURCES})
target_link_libraries(compile_test CGAL::CGAL)
target_link_libraries(compile_test Eigen3::Eigen)

target_include_directories(compile_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
