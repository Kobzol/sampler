cmake_minimum_required(VERSION 3.4)

project(sampling)

set(SOURCE_FILES
        stacktrace-collector.cpp
    sampler.cpp
    sleeptimer.cpp
    task.cpp
    taskcontext.cpp
    functionrecord.cpp
)

if (UNIX)
    set(PLATFORM_FILES
            unix/ptrace-sampler.cpp
            unix/unwind-collector.cpp
            unix/demangler.cpp
            unix/utility.cpp
    )
elseif (WIN32)
    set(PLATFORM_FILES
            win32/debugevent-sampler.cpp
            win32/stackwalk-collector.cpp
            stacktrace.cpp stacktrace.h)
endif()

add_library(sampling STATIC ${SOURCE_FILES} ${PLATFORM_FILES})
