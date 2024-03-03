add_dependencies(Matrix tabulate)
add_dependencies(Neurotic Matrix RapidXML IPCom IOPorts tabulate)

set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

target_link_libraries(Matrix PRIVATE tabulate)
target_link_libraries(Neurotic PRIVATE Matrix)
target_link_libraries(${BINARY} PRIVATE 
    tabulate
    Matrix 
    RapidXML
    IPCom
    IOPorts
    gtest_main 
)

add_test(NAME ${BINARY} COMMAND ${BINARY})