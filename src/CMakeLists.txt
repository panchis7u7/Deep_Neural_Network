add_dependencies(IPCom RapidXML RapidJson)
add_dependencies(NeuralNet Matrix)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Check for GUI compilation flag.
if(GUI_COMPILE)

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
    
    # Assign the widgets to link it with the project later if defined.
    set(QT_WIDGETS Qt${QT_VERSION_MAJOR}::Widgets)

# Perform CLI-based compilation.
else()

    set(PROJECT_SOURCES main.cpp)
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    # target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include")

endif()
unset(GUI_COMPILE CACHE)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/libs/sdplog/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER neural_net
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(IPCom PRIVATE spdlog::spdlog)
target_link_libraries(IOPorts PRIVATE IPCom spdlog::spdlog)
target_link_libraries(NeuralNet PRIVATE Matrix spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${QT_WIDGETS} 
    NeuralNet 
    Matrix 
    IPCom
    IOPorts
    RapidXML
    spdlog::spdlog
)
