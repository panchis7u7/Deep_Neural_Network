add_dependencies(NeuralNet Matrix RapidXML IPCom IOPorts)

set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

target_link_libraries(NeuralNet PRIVATE Matrix)
target_link_libraries(${BINARY} PRIVATE 
    Matrix 
    RapidXML
    IPCom
    IOPorts
    gtest_main 
)

add_test(NAME ${BINARY} COMMAND ${BINARY})