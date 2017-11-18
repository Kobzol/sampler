cmake_minimum_required(VERSION 3.4)

project(sampling)

set(SOURCE_FILES
    sampler.cpp
    sleeptimer.cpp
    stacktrace.cpp
    stacktrace-collector.cpp
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
            unix/cputime-collector.cpp
    )
elseif (WIN32)
    set(PLATFORM_FILES
            win32/debugevent-sampler.cpp
            win32/stackwalk-collector.cpp
    )
endif()

add_library(sampling STATIC ${SOURCE_FILES} ${PLATFORM_FILES})
