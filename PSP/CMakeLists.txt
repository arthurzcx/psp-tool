cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(PSP)

find_package(Qt5
        COMPONENTS
        Core
        Gui
        Widgets
        Quick
        Qml
        QuickWidgets
        Charts
        DataVisualization
        REQUIRED
)

include_directories(
        ${QT_INCLUDE_DIR}
        ../include
        ../include/json/include
        ${OPENCV_INC}
)

QT5_WRAP_CPP(
        MOC_FILES
        mainwindow.h
        manageproject.h
        page_edit_filter.h
        page_frequency.h
        page_histogram.h
        page_image_alignment.h
        page_paint_calibration.h
        page_paint_calibration_data_collection.h
        page_paint_calibration_new_project.h
        page_paint_calibration_save.h
        page_project_new.h
        page_project_process.h
        page_pseudo_config.h
        page_sectional_view.h
        page_threshold.h
        page_two_image_calculation.h
        psp_edit_transform.h
        psp_operator_util.h
        psp_paint_calibration_data_from_disk.h
        psp_system_config_dialog.h
        dialog_auto_sort_makers.h
        dlg_op_marker_add_size.h
        dlg_paint_cali_save_param.h
        ../include/base/base_log.h
        ../include/io/io.h
        ../include/manager/manager.h
        ../include/manager/work_manager.h
        ../include/manager/manager_doc.h
        ../include/op/op_base.h
        ../include/op/op_length_measure.h
        ../include/op/op_marker_pin.h
        ../include/op/op_marker_translate.h
        ../include/op/op_markeradd.h
        ../include/op/op_markerdelete.h
        ../include/op/op_paint_calibration_range.h
        ../include/op/op_roi_add.h
        ../include/op/op_sectional_view.h
        ../include/view/view_disp.h
        ../include/view/view_display_widget.h
)

qt5_add_big_resources(SOURCES res-icon.qrc)
qt5_add_big_resources(SOURCES res-lang.qrc)
qt5_add_big_resources(SOURCES qml.qrc)
SOURCE_GROUP("qrc" ${SOURCES})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

FILE(GLOB SRC_FILES "./*.cpp")
FILE(GLOB HEAD_FILES "./*.h")
FILE(GLOB UI_FILES "./*.ui")

source_group("Ui Files" FILES ${UI_FILES} ${WRAP_FILES} )

add_executable(PSP
        ${MOC_FILES}
        ${SOURCES}
        main.cpp
        mainwindow.cpp
        manageproject.cpp
        psp_edit_transform.cpp
        psp_system_config_dialog.cpp
        psp_operator_util.cpp
        dlg_op_marker_add_size.cpp
        psp_view_manager.cpp
        dialog_auto_sort_makers.cpp
        dlg_paint_cali_save_param.cpp
        page_paint_calibration.cpp
        page_image_alignment.cpp
        page_project_new.cpp
        psp_utils_json_process.cpp
        page_paint_calibration_save.cpp
        page_edit_filter.cpp
        page_threshold.cpp
        page_sectional_view.cpp
        page_histogram.cpp
        page_frequency.cpp
        logic_roi_io.cpp
        page_two_image_calculation.cpp
        page_project_features.cpp
        page_project_process.cpp
        page_pseudo_config.cpp
        page_paint_calibration_new_project.cpp
        page_paint_calibration_data_collection.cpp
        psp_paint_calibration_data_from_disk.cpp
        mainwindow_test.cpp
)

target_link_libraries(
        PSP
        PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Quick
        Qt5::Qml
        Qt5::QuickWidgets
        Qt5::Charts
        Qt5::DataVisualization
        base
        op
        manager
        algo
        io
        view
        opencv_core
        opencv_imgproc
        opencv_ximgproc
        opencv_highgui
        opencv_xfeatures2d
        opencv_calib3d
        opencv_imgcodecs
)

include(../cmake/common.cmake)

