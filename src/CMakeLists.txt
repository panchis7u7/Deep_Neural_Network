add_dependencies(NeuralNet Matrix RapidXML RS232Comm)

add_executable(${PROJECT_NAME} main.cpp "Platform/Platform.hpp")

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_link_libraries(NeuralNet PRIVATE Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE NeuralNet)
target_link_libraries(${PROJECT_NAME} PRIVATE RapidXML)
target_link_libraries(${PROJECT_NAME} PRIVATE RS232Comm)