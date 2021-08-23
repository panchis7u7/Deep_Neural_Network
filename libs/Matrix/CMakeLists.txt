set(LIBRARY_NAME Matrix)

add_library(${LIBRARY_NAME} STATIC include/Matrix.hpp src/Matrix.cpp)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)