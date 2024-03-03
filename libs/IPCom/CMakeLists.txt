set(target_headers
    Platform.hpp
    LockGuard.hpp
    Semaphore.hpp
    SharedMessage.hpp
    SpinLock.hpp
    SharedBufferQueue.hpp
    BufferQueue.hpp
    SharedAlloc.hpp
)
list(TRANSFORM target_headers PREPEND "include/")

set(target_sources
    SharedBufferQueue.cpp
    BufferQueue.cpp
)
list(TRANSFORM target_sources PREPEND "src/")

add_library(IPCom SHARED
    ${target_headers}
    ${target_sources}
)

target_sources(IPCom PRIVATE ${target_sources})

target_include_directories(IPCom
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)