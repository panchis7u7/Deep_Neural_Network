cmake_minimum_required (VERSION 3.18.4)

project (DeepNeuralNetwork)

#C++ Version to use in build.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic-errors")

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

add_definitions("-pedantic-errors -Werror")

add_executable(${PROJECT_NAME} ${SOURCES})

