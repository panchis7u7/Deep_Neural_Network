cmake_minimum_required (VERSION 3.18.4)

project (DeepNeuralNetwork)

#C++ Version to use in build.
set(CMAKE_CXX_STANDARD 20)

#Set default compile flags for GCC.
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC Detected!, adding compile flags!")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic-errors")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories (include)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list include/*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

#add_definitions("-std=c++2a")

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

