cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(base)
add_library(base SHARED)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(
        base
        PRIVATE
        Qt5::Core
)

include_directories(
        ${QT_INCLUDE_DIR}
        ../include
        ../include/json/include
)

target_sources(
        base
        PRIVATE
        base_markerpoint.cpp
        base_log.cpp
        base_sort_points.cpp
        base_graphic.cpp
        base_graphic_rect.cpp
        base_graphic_manager.cpp
        base_system_param.cpp
        base_markerpoint_sort.cpp
        base_graphic_line.cpp
        base_graphic_text.cpp
        base_graphic_colorbar.cpp
        base_arithmatic.cpp
        base_graphic_polygon.cpp
        base_def.cpp
)

add_compile_definitions(BASE_LIBRARY)

include(../cmake/common.cmake)
