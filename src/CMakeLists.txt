add_dependencies(DNN Matrix)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(DNN PRIVATE Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE DNN)
target_link_libraries(${PROJECT_NAME} PRIVATE RapidXML)