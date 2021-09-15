cmake_minimum_required(VERSION 3.18.4)

project(NeuralNetwork CXX)

#C++ Version to use in build.
set(CMAKE_CXX_STANDARD 20)

#Compilador a usar.
message ("${CMAKE_CXX_COMPILER}")

#Set default compile flags for G++.
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "G++ Detected!, adding compile flags!")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -std=c++2a")
endif(CMAKE_COMPILER_IS_GNUCXX)

#https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
#Automatically __declspec(import) and __declspec(export) by creating a .def file. 
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(libs)
add_subdirectory(src)
