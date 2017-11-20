set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -pthread")

enable_testing()

if (NOT TARGET Catch)
    include(ExternalProject)
    ExternalProject_Add(Catch-External
            URL https://github.com/catchorg/Catch2/archive/v2.0.1.tar.gz
            URL_HASH MD5=2080f4696579351d9323b3b5a8c3c71b
            PREFIX ${CMAKE_BINARY_DIR}/external/Catch
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/external/Catch/src/Catch-External/single_include/catch.hpp
            ${CMAKE_BINARY_DIR}/external/Catch/include/catch.hpp
    )
    add_library(Catch INTERFACE)
    add_dependencies(Catch Catch-External)
    target_include_directories(Catch INTERFACE ${CMAKE_BINARY_DIR}/external/Catch/include)
endif()

set(TEST_FILES TestMain.cpp Utility.cpp Utility.h TestAttach.cpp)

add_executable(sampler-test ${TEST_FILES})
target_link_libraries(sampler-test sampler Catch ${PLATFORM_LIBS})
add_test(NAME TestSampler COMMAND sampler-test)

add_subdirectory("program")
