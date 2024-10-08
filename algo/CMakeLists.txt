cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(algo)
add_library(algo SHARED)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(
        algo
        PRIVATE
        Qt5::Core
        base
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_xfeatures2d
        opencv_calib3d
)

add_compile_definitions(PSP_ALGO_LIBRARY)

include_directories(
        ${QT_INCLUDE_DIR}
        ../include
        ../include/json/include
        ${OPENCV_INC}
)

target_sources(
        algo
        PRIVATE
        algo.cpp
        algo_filter.cpp
        algo_basic_arithmatic.cpp
        algo_marker_detector.cpp
        algo_2d_features.cpp
        algo_enhancement.cpp
        algo_match.cpp
        algo_threshold.cpp
        algo_transformation.cpp
        algo_solve.cpp
        algo_marker_fill.cpp
        algo_type.cpp
)

include(../cmake/common.cmake)
