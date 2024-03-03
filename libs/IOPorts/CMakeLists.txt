set(target_headers
    Platform.hpp
    PortUtils.hpp
    AbstractPort.hpp
    SerialPort.hpp
    SerialBuffer.hpp
)
list(TRANSFORM target_headers PREPEND "include/")

set( target_sources
    SerialPortWin.cpp
    SerialPortLinux.cpp
    SerialPortMacos.cpp
    SerialBuffer.cpp
)
list(TRANSFORM target_sources PREPEND "src/")

add_library( IOPorts SHARED
    ${target_headers}
    ${target_sources}
)

target_sources(IOPorts PRIVATE ${target_sources})

target_include_directories(IOPorts
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)