cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(op)
add_library(op SHARED)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(
        op
        PRIVATE
        Qt5::Core
        manager
        opencv_core
        opencv_imgproc
        opencv_highgui
)

add_compile_definitions(OP_LIBRARY)

include_directories(
        ${QT_INCLUDE_DIR}
        ../include
        ../include/json/include
        ${OPENCV_INC}
)

target_sources(
        op
        PRIVATE
        op_undo_redo_markeradd.cpp
        op_undo_redo_stack.cpp
        op_manager.cpp
        op_factory.cpp
        op_general.cpp
        op_markeradd.cpp
        op_markerdelete.cpp
        op_paint_calibration_range.cpp
        op_sectional_view.cpp
        op_roi_add.cpp
        op_marker_pin.cpp
        op_length_measure.cpp
        op_marker_translate.cpp
)

include(../cmake/common.cmake)
