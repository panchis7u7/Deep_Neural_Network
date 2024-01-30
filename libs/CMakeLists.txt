include_directories(${RAPIDJSON_INCLUDE_DIR})
include("${CMAKE_SOURCE_DIR}/libs/RapidJson/RapidJson.cmake")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/sdplib/include")


# ###################################################################################################
# Google Test.
# ###################################################################################################
include(FetchContent)

FetchContent_Declare( 
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ###################################################################################################
# spdlog.
# ###################################################################################################

add_subdirectory(spdlog)

# ###################################################################################################
# Custom Matrix Lib.
# ###################################################################################################

add_library(
    Matrix SHARED
    Matrix/src/Matrix.cpp
    Matrix/include/Matrix.hpp
)

# ###################################################################################################
# Neural Net Lib.
# ###################################################################################################

add_library(
    NeuralNet SHARED
    NeuralNetwork/src/NeuralNetwork.cpp
    NeuralNetwork/include/NeuralNetwork.hpp
)

# ###################################################################################################
# IPCom Lib.
# ###################################################################################################

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
add_library( tabulate INTERFACE )

#install(TARGETS NeuralNets Matrix
#    RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/out/build/Win64"
#    LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}/out/build/Win64"
#)

include_directories(${RAPIDJSON_INCLUDE_DIR})

target_include_directories(Matrix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Matrix")
target_include_directories(NeuralNet PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetwork")
target_include_directories(IPCom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/IPCom")
target_include_directories(IOPorts PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/IOPorts")
target_include_directories(spdlog PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/sdplog/include")
target_include_directories(tabulate INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/tabulate/include")
target_include_directories(RapidXML INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/RapidXML")