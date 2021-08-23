cmake_minimum_required(VERSION 3.18.4)

project(DeepNeuralNetwork)

#C++ Version to use in build.
set(CMAKE_CXX_STANDARD 20)

#Set default compile flags for GCC.
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC Detected!, adding compile flags!")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(libs)
add_subdirectory(src)