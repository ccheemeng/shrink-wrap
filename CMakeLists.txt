cmake_minimum_required(VERSION 3.12...3.29)
project(ShrinkWrap)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

find_package(CGAL REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

add_executable(shrink_wrap source/shrink_wrap.cpp)
target_link_libraries(shrink_wrap CGAL::CGAL)
target_link_libraries(shrink_wrap Eigen3::Eigen)

target_include_directories(shrink_wrap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
