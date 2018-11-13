cmake_minimum_required(VERSION 3.12)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 11)

add_executable(ThreadPool main.cpp thread_pool_origin.h thread_pool.cpp thread_pool.h)

target_link_libraries(ThreadPool pthread)
