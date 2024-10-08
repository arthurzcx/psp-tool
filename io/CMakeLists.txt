cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(io)
add_library(io SHARED)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(
        io
        PRIVATE
        Qt5::Core
        base
        algo
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_imgcodecs
)

add_compile_definitions(IO_LIBRARY)

include_directories(
        ${QT_INCLUDE_DIR}
        ../include
        ../include/json/include
        ${OPENCV_INC}
)

target_sources(
        io
        PRIVATE
        io.cpp
)

include(../cmake/common.cmake)
