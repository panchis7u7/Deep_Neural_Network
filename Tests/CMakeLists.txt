add_dependencies(NeuralNet Matrix RapidXML RS232Comm)

set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

target_link_libraries(NeuralNet PRIVATE Matrix)
target_link_libraries(${BINARY} PRIVATE Matrix)
target_link_libraries(${BINARY} PRIVATE NeuralNet)
target_link_libraries(${BINARY} PRIVATE RapidXML)
target_link_libraries(${BINARY} PRIVATE RS232Comm)
target_link_libraries(${BINARY} PRIVATE gtest_main)

add_test(NAME ${BINARY} COMMAND ${BINARY})