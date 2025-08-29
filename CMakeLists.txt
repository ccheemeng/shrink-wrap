# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.

cmake_minimum_required(VERSION 3.12...3.29)
project(ShrinkWrap)

find_package(CGAL REQUIRED)

# create a target per cppfile
create_single_source_cgal_program("shrink_wrap_absolute.cpp")
create_single_source_cgal_program("shrink_wrap_relative.cpp")
