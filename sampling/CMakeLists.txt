set(SOURCE_FILES
    sleeptimer.cpp
    task.cpp
    taskcontext.cpp
    functionrecord.cpp
)

if (UNIX)
    set(PLATFORM_FILES
            unix/sampler.cpp
            unix/backstack-collector.cpp
            unix/demangler.cpp
            unix/utility.cpp
    )
endif()

add_library(sampling STATIC ${SOURCE_FILES} ${PLATFORM_FILES})
