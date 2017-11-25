cmake_minimum_required(VERSION 3.4)
project(gui)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Core Widgets Concurrent REQUIRED)
set(Qt5_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Concurrent_INCLUDE_DIRS})
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
        components/trace/TraceView.cpp
        components/trace/topdown/TopdownGroupView.cpp
        components/trace/export/ExportView.cpp
        components/settings/SettingsDialog.cpp
        model/process/ProcessLister.cpp
        model/process/Process.cpp
        model/sampler/SamplerManager.cpp
        model/util/event/event-broadcaster.h
        model/util/event/subscription-manager.cpp
        model/sampler/StartInfo.cpp
        model/settings/SettingsManager.cpp
        model/statistics/TopdownTreeCalculator.cpp
        utility/Utility.cpp
)

add_executable(gui ${SOURCE_FILES})
target_link_libraries(gui sampler Qt5::Widgets Qt5::Concurrent ${PLATFORM_LIBS})
target_include_directories(gui PRIVATE ${Qt5_INCLUDE_DIRS} "../sampler")
