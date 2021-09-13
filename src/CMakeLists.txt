add_dependencies(NeuralNets Matrix RapidXML)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(NeuralNets PRIVATE Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE NeuralNets)
target_link_libraries(${PROJECT_NAME} PRIVATE RapidXML)