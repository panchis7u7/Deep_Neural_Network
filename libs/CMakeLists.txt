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

add_library(
    RS232Comm SHARED
    RS232Comm/include/SerialPort.hpp
    RS232Comm/include/AbstractPort.hpp
    RS232Comm/include/SerialBuffer.hpp
    RS232Comm/src/SerialBuffer.cpp
    RS232Comm/include/SerialPortWin.hpp
    RS232Comm/src/SerialPortWin.cpp
)

add_library( RapidXML INTERFACE )


#install(TARGETS NeuralNets Matrix
#    RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/out/build/Win64"
#    LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}/out/build/Win64"
#)

target_include_directories(Matrix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Matrix")
target_include_directories(NeuralNets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetwork")
target_include_directories(RS232Comm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/RS232Comm")
target_include_directories(RapidXML INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/RapidXML")