cmake_minimum_required(VERSION 3.4)

project(gui)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(Qt5_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
set(CMAKE_AUTOMOC ON)

set(SOURCE_FILES
        main.cpp
        components/MainWindow.cpp
        components/command-bar/AttachDialog.cpp
        components/command-bar/CommandBar.cpp
        components/command-bar/RunProgramDialog.cpp
        components/common/tree/TreeModel.cpp
        components/common/tree/TreeItem.cpp
        components/trace/topdown/TopdownTreeView.cpp
        model/process/ProcessLister.cpp
        model/process/Process.cpp
        model/sampler/SamplerManager.cpp
        model/util/event/event-broadcaster.h
        model/util/event/subscription-manager.cpp
        model/sampler/StartInfo.cpp
        utility/Utility.cpp
        model/statistics/TopdownTreeCalculator.cpp model/statistics/TopdownTreeCalculator.h components/trace/TraceView.cpp components/trace/TraceView.h components/trace/topdown/TopdownGroupView.cpp components/trace/topdown/TopdownGroupView.h)

add_executable(gui ${SOURCE_FILES})
target_link_libraries(gui sampler Qt5::Widgets ${PLATFORM_LIBS})
target_include_directories(gui PRIVATE ${Qt5_INCLUDE_DIRS} "../sampler")
