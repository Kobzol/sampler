cmake_minimum_required(VERSION 3.4)

project(gui)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(Qt5_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
set(CMAKE_AUTOMOC ON)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(PLATFORM_LIBS unwind unwind-ptrace unwind-x86_64)
endif()

set(SOURCE_FILES
        main.cpp
        components/MainWindow.cpp
        components/command-bar/AttachDialog.cpp
        components/command-bar/CommandBar.cpp
        components/command-bar/RunProgramDialog.cpp
        model/process/ProcessLister.cpp
        model/process/Process.cpp
        model/sampler/SamplerManager.cpp
        model/util/event/event-broadcaster.h
        model/util/event/subscription-manager.cpp
        model/sampler/StartInfo.cpp model/sampler/StartInfo.h utility/Utility.cpp utility/Utility.h)

add_executable(gui ${SOURCE_FILES})
target_link_libraries(gui sampler Qt5::Widgets ${PLATFORM_LIBS})
target_include_directories(gui PRIVATE ${Qt5_INCLUDE_DIRS} "../sampler")