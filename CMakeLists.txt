cmake_minimum_required(VERSION 3.18.4)

project(Deep_Neural_Network)

#C++ Version to use in build.
set(CMAKE_CXX_STANDARD 20)

#include_directories(include)

add_subdirectory(libs)
add_subdirectory(src)