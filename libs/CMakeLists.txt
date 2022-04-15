include("${CMAKE_SOURCE_DIR}/libs/RapidJson/RapidJson.cmake")
include_directories(${RAPIDJSON_INCLUDE_DIR})

add_subdirectory(googletest)

add_library( 
    Logger SHARED
    Logger/src/Logger.cpp
    Logger/include/Logger.hpp
)

add_library(
    Matrix SHARED
    Matrix/src/Matrix.cpp
    Matrix/include/Matrix.hpp
)

add_library(
    NeuralNet SHARED
    NeuralNetwork/src/NeuralNetwork.cpp
    NeuralNetwork/include/NeuralNetwork.hpp
)

#file(GLOB IPComSources CONFIGURE_DEPENDS "IPCom/include/*.hpp" "IPCom/src/*.cpp")
#add_library( IPCom SHARED ${IPComSources})

add_library(
    IPCom SHARED
    IPCom/include/LockGuard.hpp
    IPCom/include/Semaphore.hpp
    IPCom/include/SharedMessage.hpp
    IPCom/include/SpinLock.hpp
    IPCom/include/SharedBufferQueue.hpp
    IPCom/src/SharedBufferQueue.cpp
    IPCom/include/BufferQueue.hpp
    IPCom/src/BufferQueue.cpp
    IPCom/include/SharedAlloc.hpp
)

add_library(
    IOPorts SHARED
    IOPorts/include/PortUtils.hpp
    IOPorts/include/AbstractPort.hpp
    IOPorts/include/SerialPort.hpp
    IOPorts/src/SerialPortWin.cpp
    IOPorts/src/SerialPortLinux.cpp
    IOPorts/src/SerialPortMacos.cpp
    IOPorts/include/SerialBuffer.hpp
    IOPorts/src/SerialBuffer.cpp
)

add_library( RapidXML INTERFACE )

#install(TARGETS NeuralNets Matrix
#    RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/out/build/Win64"
#    LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}/out/build/Win64"
#)

include_directories(${RAPIDJSON_INCLUDE_DIR})
target_include_directories(Logger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Logger")
target_include_directories(Matrix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Matrix")
target_include_directories(NeuralNet PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetwork")
target_include_directories(IPCom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/IPCom")
target_include_directories(IOPorts PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/IOPorts")
target_include_directories(RapidXML INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/RapidXML")
