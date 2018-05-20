cmake_minimum_required (VERSION 2.6)
project (SimpleEpoll)

set (SimpleEpoll_VERSION_MAJOR 1)
set (SimpleEpoll_VERSION_MINOR 0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -s -fno-ident  -march=native -flto -DNDEBUG")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-D_POSIX_C_SOURCE=200809L)
add_definitions(-D_DEFAULT_SOURCE)

include_directories(src)
include_directories(src/wrapper)

set(WRAPPERS
    src/wrappers/wrapper.c
    )

set(SIMPLE_SERVER
    src/simpleserver.c
    )

set(SIMPLE_CLIENT
    src/simpleclient.c
    )

add_executable(simpleserver ${SIMPLE_SERVER} ${WRAPPERS})

add_executable(simpleclient ${SIMPLE_CLIENT} ${WRAPPERS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(simpleserver rt Threads::Threads)
target_link_libraries(simpleclient rt Threads::Threads)
