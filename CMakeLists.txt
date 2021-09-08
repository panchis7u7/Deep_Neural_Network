cmake_minimum_required(VERSION 3.18.4)

project(DeepNeuralNetwork CXX)

#C++ Version to use in build.
set(CMAKE_CXX_STANDARD 20)

#Compilador a usar.
message ("${CMAKE_CXX_COMPILER}")

#Set default compile flags for G++.
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "G++ Detected!, adding compile flags!")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -std=c++2a")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(libs)
add_subdirectory(src)