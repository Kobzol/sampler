cmake_minimum_required(VERSION 3.4)

project(sampler)

set(SOURCE_FILES
    sampler.cpp
    sleeptimer.cpp
    stacktrace.cpp
    stacktrace-collector.cpp
    task.cpp
    taskcontext.cpp
    trace.cpp
    functionrecord.cpp
    export/flamechart-exporter.cpp
)

if (UNIX)
    set(PLATFORM_FILES
            unix/ptrace-sampler.cpp
            unix/collector/unwind-collector.cpp
            unix/collector/ptrace-collector.cpp
            unix/collector/dw-collector.cpp
            unix/demangler.cpp
            unix/utility/utility.cpp
            unix/utility/map-segment.cpp
            unix/cputime-collector.cpp
            unix/resolver/addrline-resolver.cpp
            unix/resolver/resolver-process.cpp
    )
elseif (WIN32)
    set(PLATFORM_FILES
            win32/debugevent-sampler.cpp
            win32/stackwalk-collector.cpp
    )
endif()

add_library(sampler STATIC ${SOURCE_FILES} ${PLATFORM_FILES})
