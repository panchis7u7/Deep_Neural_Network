set(target_headers Matrix.hpp )
list(TRANSFORM target_headers PREPEND "include/")

set(target_sources Matrix.cpp )
list(TRANSFORM target_sources PREPEND "src/")

add_library(Matrix SHARED
    ${target_headers}
    ${target_sources}
)

target_link_libraries(Matrix PRIVATE tabulate)
target_sources(Matrix PRIVATE ${target_sources})
target_include_directories(Matrix PUBLIC "${CMAKE_SOURCE_DIR}/libs/tabulate/include")

target_include_directories(Matrix
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)