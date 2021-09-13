#set(MATRIX_LIB_SRC Matrix/src/Matrix.cpp)
#set(MATRIX_LIB_HEADERS Matrix/src/Matrix.hpp)

add_library(
    Matrix SHARED
    Matrix/src/Matrix.cpp
    Matrix/include/Matrix.hpp
)

add_library(
    NeuralNets SHARED
    NeuralNetwork/src/NeuralNetwork.cpp
    NeuralNetwork/include/NeuralNetwork.hpp
)

add_library( RapidXML INTERFACE )

target_include_directories(Matrix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Matrix")
target_include_directories(NeuralNets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetwork")
target_include_directories(RapidXML INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/RapidXML")