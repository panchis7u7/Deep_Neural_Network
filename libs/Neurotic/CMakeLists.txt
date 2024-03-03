set(target_headers
    Platform.hpp
    Neurotic.hpp
)
list(TRANSFORM target_headers PREPEND "include/")

set(target_sources
    Neurotic.cpp
)
list(TRANSFORM target_sources PREPEND "src/")

add_library(Neurotic SHARED
    ${target_headers}
    ${target_sources}
)

target_sources(Neurotic PRIVATE ${target_sources})

target_include_directories(Neurotic
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)