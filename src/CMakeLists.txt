add_dependencies(IOPorts Logger)
add_dependencies(IPCom Logger RapidXML RapidJson)
add_dependencies(NeuralNet Logger Matrix)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message("Linux Operating System Detected!")
    set(CMAKE_PREFIX_PATH "/home/panchis/Qt/6.2.3/gcc_64/lib/cmake/")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message("Windows Operating System Detected!")
    set(CMAKE_PREFIX_PATH "C:\\Qt\\6.2.3\\msvc2019_64\\lib\\cmake\\")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message("MacOS Operating System Detected!")
    set(CMAKE_PREFIX_PATH "/Users/semadrig/Qt/6.2.3/macos/lib/cmake/")
endif()

message("Looking for QT...")
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        include/MainWindow.hpp
        ui/MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
endif()

#add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER neural_net
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(IPCom PRIVATE Logger)
target_link_libraries(IOPorts PRIVATE Logger IPCom)
target_link_libraries(NeuralNet PRIVATE Logger Matrix)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    NeuralNet 
    Matrix 
    IPCom
    IOPorts
    RapidXML 
)
