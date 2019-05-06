cmake_minimum_required(VERSION 3.10)
project(my_stl)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. SRC_DIR)

add_executable(my_stl ${SRC_DIR})