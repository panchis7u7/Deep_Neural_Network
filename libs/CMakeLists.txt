include_directories(${RAPIDJSON_INCLUDE_DIR})
include("${CMAKE_SOURCE_DIR}/libs/RapidJson/RapidJson.cmake")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/spdlib/include")


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
# 3rd Party libs.
# ###################################################################################################

add_library(RapidXML INTERFACE)
include_directories(${RAPIDJSON_INCLUDE_DIR})
target_include_directories(RapidXML INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/RapidXML/include")

add_library(tabulate INTERFACE)
target_include_directories(tabulate INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/tabulate/include")

# ###################################################################################################
# spdlog.
# ###################################################################################################

add_subdirectory(spdlog)
target_include_directories(spdlog PUBLIC " ${CMAKE_CURRENT_SOURCE_DIR}/sdplog/include")

# ###################################################################################################
# Custom Matrix Lib.
# ###################################################################################################

add_subdirectory(Matrix)

# ###################################################################################################
# Custom IO Ports Lib.
# ###################################################################################################

add_subdirectory(IOPorts)

# ###################################################################################################
# Custom IPCom Lib.
# ###################################################################################################

add_subdirectory(IPCom)

# ###################################################################################################
# Neural Net Lib - Neurotic.
# ###################################################################################################

add_subdirectory(Neurotic)

#install(TARGETS NeuralNets Matrix
#    RUNTIME DESTINATION " ${PROJECT_SOURCE_DIR}/out/build/Win64 "
#    LIBRARY DESTINATION " ${PROJECT_SOURCE_DIR}/out/build/Win64 "
#)