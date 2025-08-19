cmake_minimum_required(VERSION 3.12...3.29)
project(HELLO_WORLD)

find_package(CGAL REQUIRED)

create_single_source_cgal_program("hello_world.cpp")