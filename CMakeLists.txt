cmake_minimum_required(VERSION 3.4)
project(sampler)

set(CMAKE_CXX_STANDARD 14)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")
    set(PLATFORM_LIBS unwind unwind-ptrace unwind-x86_64 dw)
endif()

add_subdirectory("sampler")
add_subdirectory("gui")
add_subdirectory("test")
