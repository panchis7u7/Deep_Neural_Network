#set(MATRIX_LIB_SRC Matrix/src/Matrix.cpp)
#set(MATRIX_LIB_HEADERS Matrix/src/Matrix.hpp)

add_library(
    Matrix SHARED
    Matrix/src/Matrix.cpp
    Matrix/src/Matrix.hpp
)

add_library(
    DNN SHARED
    NeuralNetwork/src/NeuralNetwork.cpp
    NeuralNetwork/src/NeuralNetwork.hpp
)

target_include_directories(Matrix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Matrix")
target_include_directories(DNN PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetwork")