cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(manager)
add_library(manager SHARED)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(
        manager
        PRIVATE
        Qt5::Core
        base
        algo
        io
        opencv_core
        opencv_imgproc
        opencv_highgui
)

add_compile_definitions(MANAGER_LIBRARY)

include_directories(
        ${QT_INCLUDE_DIR}
        ../include
        ../include/json/include
        ${OPENCV_INC}
)

target_sources(
        manager
        PRIVATE
        manager.cpp
        work_manager.cpp
        manager_doc.cpp
        manager_doc_single_component.cpp
        manager_doc_two_component.cpp
        manager_doc_paint_calibration.cpp
)

include(../cmake/common.cmake)
