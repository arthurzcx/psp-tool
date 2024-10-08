/*
 * Copyright (C) 2024 Arthur Zhang <arthur.zhang.cx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <qdebug.h>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>
#include <QDateTime>
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QProcess>
#include <string>
#include <QCoreApplication>

#include "algo/algo_basic_arithmatic.h"
#include "algo/algo_filter.h"
#include "algo/algo_marker_detector.h"
#include "algo/algo_enhancement.h"
#include "algo/algo_type.h"

#include "base/base_log.h"
#include "base/base_trace.h"
#include "base/base_markerpoint_sort.h"
#include "base/base_graphic_rect.h"

#include "op/op_manager.h"
#include "op/op_markeradd.h"
#include "op/op_factory.h"
#include "op/op_roi_add.h"

#include "manager/manager_doc_two_component.h"
#include "manageproject.h"
#include "page_edit_filter.h"
#include "psp_edit_transform.h"
#include "psp_system_config_dialog.h"
#include "psp_view_manager.h"
#include "dialog_auto_sort_makers.h"
#include "dlg_op_marker_add_size.h"
#include "dlg_paint_cali_save_param.h"
#include "page_paint_calibration.h"
#include "page_image_alignment.h"
#include "page_project_new.h"
#include "page_paint_calibration_save.h"
#include "page_threshold.h"
#include "page_sectional_view.h"
#include "page_histogram.h"
#include "page_frequency.h"
#include "logic_roi_io.h"
#include "page_two_image_calculation.h"
#include "page_project_features.h"
#include "page_project_process.h"
#include "page_pseudo_config.h"
#include "page_paint_calibration_new_project.h"
#include "page_paint_calibration_data_collection.h"
#include "psp_paint_calibration_data_from_disk.h"

using namespace PspNsBase;
using namespace PspNsAlgo;
using namespace PspNsOp;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        mImgManager(new ImgManager()),
        mIsDocumentModified(false),
        mViewWidget(nullptr),
        mViewDisp(nullptr),
        mMainLayout(nullptr),
        mHBoxLayout1(nullptr),
        mHBoxLayout2(nullptr),
        mTreeProperty(nullptr),
        mListLog(nullptr),
        mLabelMousePos(nullptr),
        mOperatorUitl(nullptr),
        mCurrentDocument(nullptr) {
    ui->setupUi(this);

    readSettings();

    setWindowIcon(QIcon(tr(":/res-icon/window-2.png")));

    InitLayout();

    InitLanguage();

    InitOperator();

    connect(Log::getInstance(), SIGNAL(sendLogMsg(QString)), this, SLOT(updateStatus(QString)));

    connect(WorkImgManager::GetInstance(), SIGNAL(updateMarkerPoints()), this, SLOT(updateTreeProperty()));
    connect(WorkImgManager::GetInstance(), SIGNAL(updateMarkerPoints()), this, SLOT(updateMarkerPoints()));
    connect(WorkImgManager::GetInstance(), SIGNAL(updateMat()), this, SLOT(updateTreeProperty()));
    connect(WorkImgManager::GetInstance(), SIGNAL(updateMat()), this, SLOT(updateDrawMat()));
    connect(WorkImgManager::GetInstance(), SIGNAL(updateROI()), this, SLOT(updateROI()));
}

MainWindow::~MainWindow() {
    delete ui;

    delete mImgManager;
    mImgManager = nullptr;
}

void MainWindow::InitLayout() {
    InitMainToolBar();

    InitView();
    InitStatusBar();
    InitMenu();
}

void MainWindow::InitLanguage() {
    mTranslator = new QTranslator();
    if (QLocale::system().name() == "zh_CN" || QLocale::system().name() == "en_US") {
        mTranslator->load(":/res-lang/lang_" + QLocale::system().name() + ".qm");
    } else {
        mTranslator->load(":/res-lang/lang_en_US.qm");
    }

    connect(ui->actionChinese, SIGNAL(triggered()), this, SLOT(chandLanguageChinese()));
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(chandLanguageEnglish()));

    translateUi();
}

void MainWindow::InitOperator() {
    connect(OperatorFactory::CreateOperator(OperatorType::eOperatorSectionalView),
            SIGNAL(drawSectionalView(QPointF, QPointF)), this, SLOT(sectionalViewProcess(QPointF, QPointF)));

    connect(OperatorFactory::CreateOperator(OperatorType::eOperatorPaintCalibrationRange),
            SIGNAL(signalPaintCalibrationRangeSaveFromGraphicManager()),
            this,
            SLOT(paintCalibraionRangeFromGraphicManager()));
}

void MainWindow::InitView() {
    mViewWidget = new ViewDisplayWidget(ui->centralWidget);

    mMainLayout = new QVBoxLayout;
    mHBoxLayout1 = new QHBoxLayout;
    mHBoxLayout2 = new QHBoxLayout;

    mHBoxLayout1->addWidget(mViewWidget);

    mTreeProperty = new QTreeWidget(ui->centralWidget);
    mTreeProperty->installEventFilter(this);
    mListLog = new QListWidget(ui->centralWidget);
    mListLog->installEventFilter(this);

    mTreeProperty->setFixedWidth(300);
    mTreeProperty->setColumnCount(2);
    mTreeProperty->setColumnWidth(0, 100);
    mTreeProperty->setColumnWidth(1, 120);
    mTreeProperty->setHeaderLabels(QStringList() << tr("key") << tr("value"));

    mListLog->setFixedHeight(120);

    mHBoxLayout1->addWidget(mTreeProperty);
    mHBoxLayout2->addWidget(mListLog);

    mMainLayout->addLayout(mHBoxLayout1);
    mMainLayout->addLayout(mHBoxLayout2);

    mViewWidget->setMinimumSize(300, 200);
    mViewDisp = new ViewDisp(mViewWidget);
    mViewWidget->setDispView(mViewDisp);
    PspViewManager::GetInstance()->SetView(mViewDisp);

    ui->centralWidget->setLayout(mMainLayout);

    connect(mViewDisp, SIGNAL(signal_update_status(QString)), this, SLOT(updateStatus(QString)));
    connect(mViewDisp, SIGNAL(quit()), this, SLOT(emitQuit()));
    connect(mViewDisp, SIGNAL(signal_display_colorbar_config()), this, SLOT(viewPseudoConfig()));
}

void MainWindow::InitMenu() {
    InitFileMenu();
    InitViewMenu();
    InitEditMenu();
    InitROIMenu();
    InitPaintCalibration();
    InitMarkerMenu();
    InitOptionsMenu();
    InitProcessingMenu();
    InitToolsMenu();
    InitQpedMenu();
    Init3DDeformationMenu();
    InitBConvertMenu();
    InitHelpMenu();
    InitTestMenu();
    InitTest2Menu();
    InitTest3Menu();
}

void MainWindow::InitFileMenu() {
    for (int i = 0; i < MaxRecentFileCount; i++) {
        mRecentFilesAction[i] = new QAction(this);
        mRecentFilesAction[i]->setVisible(false);

        connect(mRecentFilesAction[i], SIGNAL(triggered(bool)), this, SLOT(openRecentFile()));

        ui->menuFile->addAction(mRecentFilesAction[i]);
    }

    updateRecentFileActions();

    ui->menuFile->addSeparator();
    actionExit = new QAction(tr("&Exit"), this);
    actionExit->setShortcut(QKeySequence(tr("Ctrl+Q")));
    actionExit->setStatusTip(tr("Exit app."));
    ui->menuFile->addAction(actionExit);

    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(fileNew()));
    connect(ui->actionOepn, SIGNAL(triggered(bool)), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(fileSave()));
    connect(ui->actionSave_as, SIGNAL(triggered(bool)), this, SLOT(fileSaveAs()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(fileClose()));
    connect(ui->actionExport, SIGNAL(triggered(bool)), this, SLOT(fileExport()));
    connect(ui->actionExportView, SIGNAL(triggered(bool)), this, SLOT(fileExportView()));

    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(ui->actionSystem_Set, SIGNAL(triggered(bool)), this, SLOT(configSystemParam()));

    ui->actionImport->setVisible(false);

    if (PspNsBase::SystemParam::GetInstance()->IsDemoVersion()) {
        ui->actionSave->setEnabled(false);
        ui->actionSave_as->setEnabled(false);
        ui->actionExport->setEnabled(false);
        ui->actionExportView->setEnabled(false);
    }
}

void MainWindow::InitViewMenu() {
    ui->menuView->menuAction()->setVisible(true);

    connect(ui->actionZoom_in, SIGNAL(triggered(bool)), this, SLOT(viewZoomIn()));
    connect(ui->actionZoom_out, SIGNAL(triggered(bool)), this, SLOT(viewZoomOut()));
    connect(ui->actionPrevious, SIGNAL(triggered(bool)), this, SLOT(viewPreview()));
    connect(ui->actionNext, SIGNAL(triggered(bool)), this, SLOT(viewNext()));
    connect(ui->actionOrigin, SIGNAL(triggered(bool)), mViewDisp, SLOT(resetView()));
    connect(ui->actionViewPseudo, SIGNAL(triggered(bool)), this, SLOT(viewPseudo()));
    connect(ui->actionPseudoConfig, SIGNAL(triggered(bool)), this, SLOT(viewPseudoConfig()));
    connect(ui->actionViewGray, SIGNAL(triggered(bool)), this, SLOT(viewGray()));
    connect(ui->actionLength, SIGNAL(triggered(bool)), this, SLOT(viewLengthMeasure()));
    connect(ui->actionHistogram, SIGNAL(triggered(bool)), this, SLOT(histogram()));
    connect(ui->actionSectional_View, SIGNAL(triggered(bool)), this, SLOT(sectionalView()));
    connect(ui->actionFrequency_domain, SIGNAL(triggered(bool)), this, SLOT(frequencyDomainView()));

    ui->actionPrevious->setVisible(false);
    ui->actionNext->setVisible(false);

    if (PspNsBase::SystemParam::GetInstance()->IsDemoVersion()) {
        ui->actionSectional_View->setEnabled(false);
    }
}

void MainWindow::InitEditMenu() {
    connect(ui->actionFilter, SIGNAL(triggered(bool)), this, SLOT(editFilter()));
    connect(ui->actionTransform, SIGNAL(triggered(bool)), this, SLOT(editTransform()));
    connect(ui->actionRotate, SIGNAL(triggered(bool)), this, SLOT(editRotate()));
    connect(ui->actionThresholdImage, SIGNAL(triggered(bool)), this, SLOT(editThreshold()));
    connect(ui->actionDilate, SIGNAL(triggered(bool)), this, SLOT(editDilate()));
    connect(ui->actionThin, SIGNAL(triggered(bool)), this, SLOT(editThin()));

    connect(ui->actionCanny, SIGNAL(triggered(bool)), this, SLOT(editEdgeCanny()));
    connect(ui->actionSobel_2, SIGNAL(triggered(bool)), this, SLOT(editEdgeSobel()));
    connect(ui->actionRobert, SIGNAL(triggered(bool)), this, SLOT(editEdgeRobert()));
    connect(ui->actionPrewitt, SIGNAL(triggered(bool)), this, SLOT(editEdgePrewitt()));
    connect(ui->actionLaplacian, SIGNAL(triggered(bool)), this, SLOT(editEdgeLaplaccian()));
    connect(ui->actionContour, SIGNAL(triggered(bool)), this, SLOT(editEdgeContour()));
    connect(ui->actionHist_equalization, SIGNAL(triggered(bool)), this, SLOT(editHistEqualization()));

    ui->actionArithmetic->setVisible(false);
    ui->actionCut_off_intensities->setVisible(false);
    ui->actionTransform->setVisible(false);
    ui->actionFlood_ROi->setVisible(false);
    ui->actionRotate->setVisible(false);
    ui->actionFilp_up->setVisible(false);
    ui->actionFilter_Threshold->setVisible(false);
    ui->actionRobert->setVisible(false);
    ui->actionPrewitt->setVisible(false);

    ui->actionCut->setVisible(false);
    ui->actionCopy->setVisible(false);
    ui->actionPaste->setVisible(false);
    ui->actionUndo->setVisible(false);
    ui->actionUndo_2->setVisible(false);
    ui->actionRedo->setVisible(false);
    ui->actionDelete->setVisible(false);

    if (SystemParam::GetInstance()->IsDemoVersion()) {
        ui->actionThresholdImage->setEnabled(false);
    }
}

void MainWindow::InitROIMenu() {
    connect(ui->actionOpen_ROI, SIGNAL(triggered(bool)), this, SLOT(roiOpen()));
    connect(ui->actionSave_ROI_as, SIGNAL(triggered(bool)), this, SLOT(roiSave()));
    connect(ui->actionROI_by_boxes, SIGNAL(triggered(bool)), this, SLOT(roiByBox()));
    connect(ui->actionROI_by_polygons, SIGNAL(triggered(bool)), this, SLOT(roiByPolygon()));

    if (!SystemParam::GetInstance()->IsDebugMode()) {
        ui->actionROI_all->setVisible(false);
        ui->actionInvert_ROI->setVisible(false);
    }

    if (SystemParam::GetInstance()->IsDemoVersion()) {
        ui->menuROI->menuAction()->setEnabled(false);
    }
}

void MainWindow::InitPaintCalibration() {
    connect(ui->actionSave_range, SIGNAL(triggered(bool)), this, SLOT(savePaintCalibrationRange()));
    connect(ui->actionSave_parameter, SIGNAL(triggered(bool)), this, SLOT(savePaintCalibrationParameter()));
    connect(ui->actionPaintCalibration, SIGNAL(triggered(bool)), this, SLOT(paintCalibration()));

    ui->actionNew_project->setEnabled(true);
    ui->actionView_project->setEnabled(true);
    ui->actionSave_project->setEnabled(true);
    ui->actionOpen_project->setEnabled(true);
    ui->actionData_collection->setEnabled(true);

    connect(ui->actionNew_project, SIGNAL(triggered(bool)), this, SLOT(paintCalibrationNewProject()));
    connect(ui->actionView_project, SIGNAL(triggered(bool)), this, SLOT(paintCalibrationViewProject()));
    connect(ui->actionSave_project, SIGNAL(triggered(bool)), this, SLOT(paintCalibrationSaveProject()));
    connect(ui->actionOpen_project, SIGNAL(triggered(bool)), this, SLOT(paintCalibrationOpenProject()));
    connect(ui->actionData_collection, SIGNAL(triggered(bool)), this, SLOT(paintCalibrationDataCollection()));
}

void MainWindow::InitMarkerMenu() {
    connect(ui->actionMark, SIGNAL(triggered(bool)), this, SLOT(addMarkerManual()));
    connect(ui->actionDelete_Marker, SIGNAL(triggered(bool)), this, SLOT(deleteMarkerManual()));
    connect(ui->actionAuto_Mark, SIGNAL(triggered(bool)), this, SLOT(autoMarker()));
    connect(ui->actionReferenceMark, SIGNAL(triggered(bool)), this, SLOT(referenceMarker()));
    connect(ui->actionSave_Markers_As, SIGNAL(triggered(bool)), this, SLOT(saveMarkerFile()));
    connect(ui->actionOpen_Markers, SIGNAL(triggered(bool)), this, SLOT(readMarkerFile()));
    connect(ui->actionHide_Markers, SIGNAL(triggered(bool)), this, SLOT(hideMarker()));
    connect(ui->actionAuto_sort, SIGNAL(triggered(bool)), this, SLOT(autoSort()));
    connect(ui->actionFill_Up_Markers, SIGNAL(triggered(bool)), this, SLOT(fillupMarkers()));
    connect(ui->actionPin_Mark, SIGNAL(triggered(bool)), this, SLOT(pinMarker()));
    connect(ui->actionTranslate_marker, SIGNAL(triggered(bool)), this, SLOT(translateMarker()));

    ui->actionClose_Markers->setVisible(false);
    ui->actionRevise_Statistics->setVisible(false);

    if (SystemParam::GetInstance()->IsDemoVersion()) {
        ui->actionReferenceMark->setEnabled(false);
        ui->actionFill_Up_Markers->setEnabled(false);
    }
}

void MainWindow::InitOptionsMenu() {
    ui->menuOptions->menuAction()->setVisible(false);
}

void MainWindow::InitProcessingMenu() {
    connect(ui->actionSingle_Component, SIGNAL(triggered(bool)), this, SLOT(newProjectSinglComponent()));
    connect(ui->actionDual_Component, SIGNAL(triggered(bool)), this, SLOT(newProjectDualComponent()));
    connect(ui->actionProjectFeatures, SIGNAL(triggered(bool)), this, SLOT(projectFeatures()));
    connect(ui->actionOpenProject, SIGNAL(triggered(bool)), this, SLOT(openProject()));
    connect(ui->actionSaveProject, SIGNAL(triggered(bool)), this, SLOT(saveProject()));
    connect(ui->actionCloseProject, SIGNAL(triggered(bool)), this, SLOT(closeProject()));
    connect(ui->actionManager_Project, SIGNAL(triggered(bool)), this, SLOT(manageProject()));
    connect(ui->actionView_Project, SIGNAL(triggered(bool)), this, SLOT(viewProject()));
    connect(ui->actionLight_Ratio, SIGNAL(triggered(bool)), this, SLOT(lightRatio()));
    connect(ui->actionPressure_View, SIGNAL(triggered(bool)), this, SLOT(pressureView()));
    connect(ui->actionPressureCoefficientView, SIGNAL(triggered(bool)), this, SLOT(pressureCoefficientView()));
    connect(ui->actionProcessProject, SIGNAL(triggered(bool)), this, SLOT(processProject()));
    connect(ui->actionSaveProjectResults, SIGNAL(triggered(bool)), this, SLOT(saveProjectResults()));

    connect(ui->actionWindOn, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageWindOn()));
    connect(ui->actionWindOff, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageWindOff()));
    connect(ui->actionWindOn2, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageWindOn2()));
    connect(ui->actionWindOff2, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageWindOff2()));
    connect(ui->actionBackground, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageBackground()));
    connect(ui->actionBackground2, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageBackground2()));
    connect(ui->actionBackground_3, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageBackground3()));
    connect(ui->actionBackground_4, SIGNAL(triggered(bool)), this, SLOT(projectCurrentImageBackground4()));

    ui->actionOpen_Projet_and_Data->setVisible(false);
    ui->actionClose_Project_and_Data->setVisible(false);

    if (!SystemParam::GetInstance()->IsDebugMode()) {
        ui->actionMake_Field_P_Binary_Paint->setVisible(false);
        ui->actionClose_Project_and_Data->setVisible(false);
        ui->actionResect_Current_Image->setVisible(false);
        ui->actionResect_Test_Data->setVisible(false);
    }

    ui->actionSaveProjectResults->setEnabled(false);
    ui->actionView_Project->setEnabled(false);
    ui->actionManager_Project->setEnabled(false);

    if (SystemParam::GetInstance()->IsDemoVersion()) {
        ui->actionSingle_Component->setEnabled(false);
        ui->actionPressure_View->setEnabled(false);
        ui->actionPressureCoefficientView->setEnabled(false);
    }
}

void MainWindow::InitTestMenu() {
    if (!SystemParam::GetInstance()->IsDebugMode()) {
        ui->menuTest->menuAction()->setVisible(false);
        ui->menuTest->setVisible(false);
    }

    connect(ui->actionSetAsBackground, SIGNAL(triggered(bool)), this, SLOT(setImgAsBackgound()));
    connect(ui->actionSetAsReference, SIGNAL(triggered(bool)), this, SLOT(setImgAsReference()));
    connect(ui->actionSetAsPsp, SIGNAL(triggered(bool)), this, SLOT(setImgAsPsp()));

    connect(ui->actionSubtract, SIGNAL(triggered(bool)), this, SLOT(testSubtract()));
    connect(ui->actionAdd, SIGNAL(triggered(bool)), this, SLOT(testAdd()));
    connect(ui->actionDivide, SIGNAL(triggered(bool)), this, SLOT(testDivide()));
    connect(ui->actiontestAverage, SIGNAL(triggered(bool)), this, SLOT(testAverage()));

    connect(ui->actionFitEllipse, SIGNAL(triggered(bool)), this, SLOT(testFitEllipse()));
    connect(ui->actionFast_detector, SIGNAL(triggered(bool)), this, SLOT(testMarkerDetector()));
    connect(ui->actionAgast_detector, SIGNAL(triggered(bool)), this, SLOT(testAgastDetector()));;
    connect(ui->actionAkaze_detector, SIGNAL(triggered(bool)), this, SLOT(testAkazeDetector()));
    connect(ui->actionBrisk_detector, SIGNAL(triggered(bool)), this, SLOT(testBriskDetector()));
    connect(ui->actionGFTT_detector, SIGNAL(triggered(bool)), this, SLOT(testGfttDetector()));
    connect(ui->actionKaze_detector, SIGNAL(triggered(bool)), this, SLOT(testKazeDetetor()));
    connect(ui->actionMser_detector, SIGNAL(triggered(bool)), this, SLOT(testMserDetetor()));
    connect(ui->actionOrb_detector, SIGNAL(triggered(bool)), this, SLOT(testOrbDetector()));
    connect(ui->actionSimpleBlob_detector, SIGNAL(triggered(bool)), this, SLOT(testSimpleBlobDetector()));
    connect(ui->actionFilter_Enhance_Detect, SIGNAL(triggered(bool)), this, SLOT(testFilterEnhanceDetector()));

    connect(ui->actionDiplay_background_img, SIGNAL(triggered(bool)), this, SLOT(displayBackgroundImg()));
    connect(ui->actionDisplay_reference_img, SIGNAL(triggered(bool)), this, SLOT(displayReferenceImg()));
    connect(ui->actionDisplay_psp_img, SIGNAL(triggered(bool)), this, SLOT(displayPspImg()));

    connect(ui->actionView3d, SIGNAL(triggered(bool)), this, SLOT(testView3d()));

    connect(ui->actionPython_matplotlib, SIGNAL(triggered(bool)), this, SLOT(testPythonMatPlotLib()));
    connect(ui->actionPython_sendmail, SIGNAL(triggered(bool)), this, SLOT(testPythonSendMail()));

    connect(ui->actionThreshold, SIGNAL(triggered(bool)), this, SLOT(testThreshold()));
    connect(ui->actionAdaptiveThreshold, SIGNAL(triggered(bool)), this, SLOT(testAdaptiveThreshold()));

    connect(ui->actionImage16bytes, SIGNAL(triggered(bool)), this, SLOT(testImage16bytes()));
    connect(ui->actionTemplateMatching, SIGNAL(triggered(bool)), this, SLOT(testTemplateMatching()));

    connect(ui->actionCannyEdge, SIGNAL(triggered(bool)), this, SLOT(testCannyEdge()));
    connect(ui->actiontestContourDetection, SIGNAL(triggered(bool)), this, SLOT(testContourDetection()));
    connect(ui->actiontestContourFiltering, SIGNAL(triggered(bool)), this, SLOT(testContourFiltering()));
    connect(ui->actionSmoothingByFilter, SIGNAL(triggered(bool)), this, SLOT(testSmoothingByFilter()));
    connect(ui->actionSmoothingByPyramid, SIGNAL(triggered(bool)), this, SLOT(testSmoothingByPyramid()));
    connect(ui->actiontestContourFit, SIGNAL(triggered(bool)), this, SLOT(testContourFit()));

    connect(ui->actiontestHistogram, SIGNAL(triggered(bool)), this, SLOT(testHistogram()));
    connect(ui->actiontestErosion, SIGNAL(triggered(bool)), this, SLOT(testErosion()));
    connect(ui->actiontestDilation, SIGNAL(triggered(bool)), this, SLOT(testDilation()));
    connect(ui->actionOpening, SIGNAL(triggered(bool)), this, SLOT(MorphologyOpening()));
    connect(ui->actionClosing, SIGNAL(triggered(bool)), this, SLOT(MorphologyClosing()));
    connect(ui->actionMorphologicalGradient, SIGNAL(triggered(bool)), this, SLOT(MorphologyMorphologicalGradient()));
    connect(ui->actionTop_Hat, SIGNAL(triggered(bool)), this, SLOT(MorphologyTop_Hat()));
    connect(ui->actionBlack_Hat, SIGNAL(triggered(bool)), this, SLOT(MorphologyBlack_Hat()));
    connect(ui->actiontestLaplaceSharp, SIGNAL(triggered(bool)), this, SLOT(testLaplaceSharp()));

    connect(ui->actionSurfMatch, SIGNAL(triggered(bool)), this, SLOT(testSurMatch()));
    connect(ui->actionCornerSubPix_detector, SIGNAL(triggered(bool)), this, SLOT(testCornerSubPix()));

    connect(ui->actiontestHoughCircles, SIGNAL(triggered(bool)), this, SLOT(testHoughCircles()));
}

void MainWindow::InitTest2Menu() {
    if (!SystemParam::GetInstance()->IsDebugMode()) {
        ui->menuTest2->menuAction()->setVisible(false);
    }

    connect(ui->actiontestAlignment, SIGNAL(triggered(bool)), this, SLOT(testAlignment()));
    connect(ui->actiontestColorBar, SIGNAL(triggered(bool)), this, SLOT(testColorBar()));

    connect(ui->actiontestMedianFilter, SIGNAL(triggered(bool)), this, SLOT(testMedianFilter()));
    connect(ui->actiontestGaussianFilter, SIGNAL(triggered(bool)), this, SLOT(testGaussianFilter()));
    connect(ui->actiontestBilateralFilter, SIGNAL(triggered(bool)), this, SLOT(testBilateralFilter()));
    connect(ui->actionSobelX, SIGNAL(triggered(bool)), this, SLOT(testSobelX()));
    connect(ui->actionSobel, SIGNAL(triggered(bool)), this, SLOT(testSobel()));
    connect(ui->actionSobelY, SIGNAL(triggered(bool)), this, SLOT(testSobelY()));

    connect(ui->actiontestFFT, SIGNAL(triggered(bool)), this, SLOT(testFFT()));
    connect(ui->actiontestIFFT, SIGNAL(triggered(bool)), this, SLOT(testIFFT()));

    connect(ui->actiontestBackgroundSubtraction, SIGNAL(triggered(bool)), this, SLOT(testBackgroundSubtraction()));

    connect(ui->actionPyrUp, SIGNAL(triggered(bool)), this, SLOT(testPyrUp()));
    connect(ui->actionPyrDown, SIGNAL(triggered(bool)), this, SLOT(testPyrDown()));

    connect(ui->actionBilinearInterpolation, SIGNAL(triggered(bool)), this, SLOT(testBilinearInterpolation()));
    connect(ui->actionBicubicInterpolation, SIGNAL(triggered(bool)), this, SLOT(testBicubicInterpolation()));
    connect(ui->actionNearestNeighborInterpolation, SIGNAL(triggered(bool)), this,
            SLOT(testNearestNeighborInterpolation()));
    connect(ui->actionLANCZOS4, SIGNAL(triggered(bool)), this, SLOT(testLANCZOS4()));

    connect(ui->actionhistEqualization, SIGNAL(triggered(bool)), this, SLOT(testHistEqualization()));

    connect(ui->actiontestColormap, SIGNAL(triggered(bool)), this, SLOT(testColormap()));

    connect(ui->actiontestImageThinning, SIGNAL(triggered(bool)), this, SLOT(testImageThinning()));
    connect(ui->actiontestSegmentation, SIGNAL(triggered(bool)), this, SLOT(testSegmentation()));
}

void MainWindow::InitTest3Menu() {
    if (!SystemParam::GetInstance()->IsDebugMode()) {
        ui->menuTest3->menuAction()->setVisible(false);
    }
    connect(ui->actionHistGlobalThreshold, SIGNAL(triggered(bool)), this, SLOT(testHistGlobalThreshold()));
    connect(ui->actionOstuGlobalThreshold, SIGNAL(triggered(bool)), this, SLOT(testOstuGlobalThreshold()));
    connect(ui->actionEntropyGlobalThreshold, SIGNAL(triggered(bool)), this, SLOT(testEntropyGlobalThresold()));
    connect(ui->actionTriangleGlobalThreshold, SIGNAL(triggered(bool)), this, SLOT(testTriangleGlobalThreshold()));

    connect(ui->actionPaintCalibrationDataFromDisk, SIGNAL(triggered(bool)), this,
            SLOT(testPaintCalibrationDataFromDisk()));
    connect(ui->actionBrightnessContrastAdjust, SIGNAL(triggered(bool)), this, SLOT(testBrightnessContrastAdjust()));
}

void MainWindow::InitToolsMenu() {
    connect(ui->actionImage_Alignment, SIGNAL(triggered(bool)), this, SLOT(imageAlignment()));
    connect(ui->actionImageAddition, SIGNAL(triggered(bool)), this, SLOT(imageAddition()));
    connect(ui->actionImageSubtract, SIGNAL(triggered(bool)), this, SLOT(imageSbstraction()));
    connect(ui->actionImageDivision, SIGNAL(triggered(bool)), this, SLOT(imageDivision()));
    connect(ui->actionImage_average, SIGNAL(triggered(bool)), this, SLOT(imageAverage()));

    ui->actionChart->setEnabled(false);
    ui->actionSummary->setEnabled(false);
    ui->actionOptical_Distortion_Correction->setEnabled(false);
    ui->actionPressure_Ports->setEnabled(false);
    ui->actionImage_Features->setEnabled(false);
    ui->actionForeGround_Thinning->setEnabled(false);
    ui->actionForeGround_Expansion->setEnabled(false);

    ui->actionBackGround->setEnabled(false);
    ui->actionReset_BackGround->setEnabled(false);
    ui->actionThresholdTool->setEnabled(false);
    ui->actionResetThreshold->setEnabled(false);
    ui->actionDark_Field_Subtraction->setEnabled(false);
    ui->actionFlat_Field_Correction->setEnabled(false);
    ui->actionChart->setVisible(false);
    ui->actionSummary->setVisible(false);
    ui->actionDark_Field_Subtraction->setVisible(false);
}

void MainWindow::InitQpedMenu() {
    ui->menuQPED->menuAction()->setVisible(false);
}

void MainWindow::Init3DDeformationMenu() {
    ui->menu3D_Deformations->menuAction()->setVisible(false);
}

void MainWindow::InitBConvertMenu() {
    ui->menuB_Convert->menuAction()->setVisible(false);
}

void MainWindow::InitHelpMenu() {
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(aboutSoftware()));
    connect(ui->actionContents, SIGNAL(triggered(bool)), this, SLOT(helpManual()));
    connect(ui->actionWebsite, SIGNAL(triggered(bool)), this, SLOT(openWebsite()));
}

void MainWindow::InitMainToolBar() {
    ui->mainToolBar->addAction(ui->actionSystem_Set);
    ui->mainToolBar->addAction(ui->actionNew);
    ui->mainToolBar->addAction(ui->actionOepn);
    ui->mainToolBar->addAction(ui->actionSave);
    ui->mainToolBar->addAction(ui->actionSave_as);
    ui->mainToolBar->addAction(ui->actionImport);
    ui->mainToolBar->addAction(ui->actionExport);

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionZoom_in);
    ui->mainToolBar->addAction(ui->actionZoom_out);
    ui->mainToolBar->addAction(ui->actionOrigin);

    ui->actionViewPseudo->setIcon(QIcon(":/res-icon/icon-pseudo.png"));
    ui->mainToolBar->addAction(ui->actionViewPseudo);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->actionLength);
    ui->mainToolBar->addAction(ui->actionSectional_View);
    ui->mainToolBar->addAction(ui->actionHistogram);
    ui->mainToolBar->addAction(ui->actionFrequency_domain);

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionAuto_Mark);
    ui->mainToolBar->addAction(ui->actionMark);
    ui->mainToolBar->addAction(ui->actionPin_Mark);
    ui->mainToolBar->addAction(ui->actionTranslate_marker);
    ui->mainToolBar->addAction(ui->actionDelete_Marker);
}

void MainWindow::InitStatusBar() {
    mLabelMousePos = new QLabel;
    ui->statusBar->addPermanentWidget(mLabelMousePos);

    connect(mViewDisp, SIGNAL(signal_update_pos(QPointF)), this, SLOT(updatePosPixel(QPointF)));
}

QTranslator *MainWindow::GetTranslator() const {
    return mTranslator;
}

void MainWindow::chandLanguageChinese() {
    mTranslator->load(":/res-lang/lang_zh_CN.qm");

    translateUi();

    LOG_CLIENT(tr("Switch language to Chinese."));
}

void MainWindow::chandLanguageEnglish() {
    mTranslator->load(":/res-lang/lang_en_US.qm");

    translateUi();

    LOG_CLIENT(tr("Switch language to English."));
}

void MainWindow::emInfo() {
}

void MainWindow::translateUi() {
    if (!mOperatorUitl) mOperatorUitl = new PspOperatorUtil();
    mOperatorUitl->TranslateUi();

    ui->menuFile->setTitle(tr("File"));
    ui->menuView->setTitle(tr("View"));
    ui->menuEdit->setTitle(tr("Edit"));
    ui->menuROI->setTitle(tr("ROI"));
    ui->menuPaint_Calibration->setTitle(tr("Paint Caliration"));
    ui->menuMarkers->setTitle(tr("Marker"));
    ui->menuOptions->setTitle(tr("Options"));
    ui->menuTools->setTitle(tr("Tools"));
    ui->menuB_Convert->setTitle(tr("B Convert"));
    ui->menuIMS_Processing->setTitle(tr("Project"));
    ui->menuQPED->setTitle(tr("QPED"));
    ui->menu3D_Deformations->setTitle(tr("3D Deformation"));
    ui->menuHelp->setTitle(tr("Help"));
    ui->menuTest->setTitle(tr("Test"));

    ui->actionNew->setText(tr("&New"));
    ui->actionOepn->setText(tr("&Open"));
    ui->actionClose->setText(tr("Close"));
    ui->actionClose->setShortcut(QKeySequence(tr("Ctrl+W")));
    ui->actionSystem_Set->setText(tr("Sytem setting"));
    ui->actionSave->setText(tr("&Save"));
    ui->actionSave_as->setText(tr("Save as"));
    ui->actionImport->setText(tr("&Import"));
    ui->actionExport->setText(tr("&Export"));
    ui->actionExportView->setText(tr("Export view"));

    ui->actionZoom_in->setText(tr("Zoom in"));
    ui->actionZoom_in->setShortcut(QKeySequence(tr("Ctrl++")));
    ui->actionZoom_out->setText(tr("Zoom out"));
    ui->actionZoom_out->setShortcut(QKeySequence(tr("Ctrl+-")));
    ui->actionPrevious->setText(tr("Forward view"));
    ui->actionNext->setText(tr("Backward view"));

    if (SystemParam::GetInstance()->IsShowPseudoColor()) {
        ui->actionViewPseudo->setText(tr("Switch to Gray"));
        ui->actionPseudoConfig->setEnabled(true);
        ui->actionViewPseudo->setIcon(QIcon(":/res-icon/icon-pseudo.png"));
    } else {
        ui->actionViewPseudo->setText(tr("Switch to Pseudo"));
        ui->actionPseudoConfig->setEnabled(false);
        ui->actionViewPseudo->setIcon(QIcon(":/res-icon/icon-gray.png"));
    }
    ui->actionViewPseudo->setShortcut(QKeySequence(tr("Ctrl+Shift+C")));

    ui->actionViewGray->setText(tr("Gray"));
    ui->actionViewGray->setVisible(false);

    ui->actionOrigin->setText(tr("Origin"));
    ui->actionOrigin->setShortcut(QKeySequence(tr("Ctrl+Shift+O")));
    ui->actionLength->setText(tr("Length"));
    ui->actionLength->setShortcut(QKeySequence(tr("Ctrl+L")));
    ui->actionPseudoConfig->setText(tr("Pseudo config"));

    ui->actionOpen_ROI->setText(tr("Open ROI"));
    ui->actionSave_ROI_as->setText(tr("Save ROI as"));
    ui->actionROI_by_boxes->setText(tr("ROI by boxes"));
    ui->actionROI_by_polygons->setText(tr("ROI by polygons"));
    ui->actionROI_all->setText(tr("ROI all"));
    ui->actionInvert_ROI->setText(tr("Invert ROI"));

    ui->actionSave_range->setText(tr("Save range"));
    ui->actionSave_parameter->setText(tr("Save parameter"));
    ui->actionPaintCalibration->setText(tr("Paint calibration"));
    ui->actionNew_project->setText(tr("New project"));
    ui->actionNew_project->setShortcut(QKeySequence(tr("ALT+P")));
    ui->actionData_collection->setText(tr("Data collection"));
    ui->actionData_collection->setShortcut(QKeySequence(tr("ALT+C")));
    ui->actionSave_project->setText(tr("Save project"));
    ui->actionSave_project->setShortcut(QKeySequence(tr("ALT+S")));
    ui->actionOpen_project->setText(tr("Open project"));
    ui->actionOpen_project->setShortcut(QKeySequence(tr("ALT+O")));
    ui->actionManage_project->setText(tr("Manage project"));
    ui->actionView_project->setText(tr("View project"));
    ui->actionView_project->setShortcut(QKeySequence(tr("ALT+V")));

    ui->actionUndo->setText(tr("Undo"));
    ui->actionRedo->setText(tr("Redo"));
    ui->actionCopy->setText(tr("Copy"));
    ui->actionCut->setText(tr("Cut"));
    ui->actionPaste->setText(tr("Paste"));
    ui->actionDelete->setText(tr("Delete"));
    ui->actionArithmetic->setText(tr("Arithmetic"));
    ui->actionCut_off_intensities->setText(tr("Cut off intensities"));
    ui->actionTransform->setText(tr("Transform"));
    ui->actionFlood_ROi->setText(tr("Flood ROI"));
    ui->actionFilp_up->setText(tr("Flip up"));
    ui->actionFilter->setText(tr("Filter"));
    ui->actionFilter->setShortcut(QKeySequence(tr("Ctrl+Shift+F")));
    ui->actionFilter_Threshold->setText(tr("Filter threshold"));
    ui->actionRotate->setText(tr("Rotate"));
    ui->actionThresholdImage->setText(tr("Threshold"));
    ui->actionDilate->setText(tr("Dilate"));
    ui->actionDilate->setShortcut(QKeySequence(tr("Ctrl+Shift+D")));
    ui->actionThin->setText(tr("Thin"));
    ui->actionThin->setShortcut(QKeySequence(tr("Ctrl+Shift+T")));
    ui->menuEdge_and_contour->setTitle(tr("Edge"));
    ui->menuMorphology_transfrom->setTitle(tr("Morphology transform"));
    ui->actionContour->setText(tr("Contour"));
    ui->actionHist_equalization->setText(tr("Hist equalization"));
    ui->actionHist_equalization->setShortcut(QKeySequence(tr("Ctrl+E")));

    ui->actionOpening->setText(tr("Opening"));
    ui->actionClosing->setText(tr("Closing"));
    ui->actionMorphologicalGradient->setText(tr("Gradient"));
    ui->actionTop_Hat->setText(tr("Top hat"));
    ui->actionBlack_Hat->setText(tr("Black hat"));

    ui->actionCanny->setText(tr("Canny"));
    ui->actionSobel_2->setText(tr("Sobel"));
    ui->actionRobert->setText(tr("Robert"));
    ui->actionPrewitt->setText(tr("Prewitt"));
    ui->actionLaplacian->setText(tr("Laplacian"));

    ui->actionOpen_Markers->setText(tr("Open markers"));
    ui->actionOpen_Markers->setShortcut(QKeySequence(tr("Shift+O")));
    ui->actionClose_Markers->setText(tr("Close markers"));
    ui->actionSave_Markers_As->setText(tr("Save markers as"));
    ui->actionSave_Markers_As->setShortcut(QKeySequence(tr("Shift+S")));
    ui->actionHide_Markers->setText(tr("Hide markers"));
    ui->actionHide_Markers->setShortcut(QKeySequence(tr("Shift+D")));
    ui->actionMark->setText(tr("Manually mark"));
    ui->actionMark->setShortcut(QKeySequence(tr("Shift+M")));
    ui->actionDelete_Marker->setText(tr("Delete markers"));
    ui->actionAuto_Mark->setText(tr("Auto mark"));
    ui->actionAuto_Mark->setShortcut(QKeySequence(tr("Shift+A")));
    ui->actionReferenceMark->setText(tr("Reference mark"));
    ui->actionRevise_Statistics->setText(tr("Revise statitics"));
    ui->actionFill_Up_Markers->setText(tr("Fill up markers"));
    ui->actionAuto_sort->setText(tr("Auto sort"));
    ui->actionPin_Mark->setText(tr("Pin mark"));
    ui->actionPin_Mark->setShortcut(QKeySequence(tr("Shift+P")));
    ui->actionTranslate_marker->setText(tr("Translate marker"));
    ui->actionTranslate_marker->setShortcut(QKeySequence(tr("Shift+T")));

    ui->actionFashion->setText(tr("Fashion"));
    ui->actionDecoration->setText(tr("Decoration"));
    ui->actionFonts->setText(tr("Fonts"));

    ui->actionChart->setText(tr("Chart"));
    ui->actionSummary->setText(tr("Summary"));
    ui->actionDark_Field_Subtraction->setText(tr("Dark field subtraction"));
    ui->actionFlat_Field_Correction->setText(tr("Flat-field correction"));
    ui->actionOptical_Distortion_Correction->setText(tr("Lens distortion correction"));
    ui->actionBackGround->setText(tr("Threshold"));
    ui->actionReset_BackGround->setText(tr("Reset threshold"));
    ui->actionThresholdTool->setText(tr("Threshold"));
    ui->actionResetThreshold->setText(tr("Reset threshold"));
    ui->actionForeGround_Thinning->setText(tr("MorphThinning"));
    ui->actionForeGround_Expansion->setText(tr("MorphThick"));
    ui->actionImage_Alignment->setText(tr("Image alignment"));
    ui->actionImageAddition->setText(tr("Image Addition"));
    ui->actionImageDivision->setText(tr("Image Division"));
    ui->actionImageSubtract->setText(tr("Image Substraction"));
    ui->actionImage_average->setText(tr("Images average"));
    ui->actionPressure_Ports->setText(tr("Pressure taps"));
    ui->actionImage_Features->setText(tr("Image info"));
    ui->actionFrequency_domain->setText(tr("Frequency domain"));
    ui->actionFrequency_domain->setShortcut(QKeySequence(tr("Ctrl+F")));

    ui->actionOpen_Project->setText(tr("Open project"));
    ui->actionProjectFeatures->setText(tr("Project features"));
    ui->actionUndo_2->setText(tr("Undo"));
    ui->actionMarkers_Copy->setText(tr("Markers copy"));
    ui->actionMarkers_Paste->setText(tr("Markers paste"));
    ui->actionStore_Primary_Markers->setText(tr("Store primary markers"));
    ui->actionUSE_Wind_Off_Sens_Markers->setText(tr("USE wind off sens markers"));
    ui->actionSet_Confirm_Reference_Single_Paint->setText(tr("Set confirm reference single paint"));
    ui->actionDark_Frame_Subtraction->setText(tr("Dark frame subtraction"));
    ui->actionFlat_Field_Correction_2->setText(tr("Flat field correction"));
    ui->actionAutomatic_Marking->setText(tr("Automatic marking"));
    ui->actionMarkers_Precise_Positon->setText(tr("Markers precise position"));
    ui->actionNumerate_Markers->setText(tr("Numerate markers"));
    ui->actionFill_Up_All_Markers->setText(tr("Fill up all markers"));
    ui->actionFiltering->setText(tr("Filtering"));
    ui->actionBackground_Work->setText(tr("Background work"));
    ui->actionDistortion_Correction->setText(tr("Distortion correction"));
    ui->actionAlignment_Images->setText(tr("Alignment images"));
    ui->actionImage_Convert->setText(tr("Image convert"));
    ui->actionFinal_Filtering->setText(tr("Final filtering"));
    ui->actionResection->setText(tr("Resection"));
    ui->actionSave_Results->setText(tr("Save results"));
    ui->actionClose_Project->setText(tr("Close project"));
    ui->actionAutomatic_Convert->setText(tr("Automatic convert"));
    ui->actionAutomatic_Convert_List->setText(tr("Automatic convert list"));
    ui->actionProcessProject->setText(tr("Process project"));
    ui->actionProcessProject->setShortcut(QKeySequence(tr("Ctrl+Alt+P")));
    ui->actionPressureCoefficientView->setText(tr("Pressure coefficient view"));

    ui->menuNew_Project->setTitle(tr("New project"));
    ui->actionSingle_Component->setText(tr("Single component"));
    ui->actionSingle_Component->setShortcut(QKeySequence(tr("Ctrl+Alt+S")));
    ui->actionDual_Component->setText(tr("Dual component"));
    ui->actionDual_Component->setShortcut(QKeySequence(tr("Ctrl+Alt+D")));
    ui->actionManager_Project->setText(tr("Manager project"));
    ui->actionView_Project->setText(tr("View project"));
    ui->actionDual_Component->setShortcut(QKeySequence(tr("Ctrl+Alt+V")));
    ui->actionOpen_Projet_and_Data->setText(tr("Open project and data"));
    ui->actionClose_Project_and_Data->setText(tr("Close project and data"));
    ui->actionSaveProjectResults->setText(tr("Save result"));
    ui->actionMake_Field_P_Binary_Paint->setText(tr("Make field P binary paint"));
    ui->actionResect_Current_Image->setText(tr("Resect current image"));
    ui->actionResect_Test_Data->setText(tr("Resect test data"));
    ui->menuCurrent_Image->setTitle(tr("Current Image"));
    ui->actionOpenProject->setText(tr("Open project"));
    ui->actionOpenProject->setShortcut(QKeySequence(tr("Ctrl+Alt+O")));
    ui->actionSaveProject->setText(tr("Save project"));
    ui->actionSaveProject->setShortcut(QKeySequence(tr("Ctrl+Alt+S")));
    ui->actionCloseProject->setText(tr("Close project"));
    ui->actionCloseProject->setShortcut(QKeySequence(tr("Ctrl+Alt+C")));
    ui->actionBackGround->setText(tr("Background"));

    ui->actionBackground->setText(tr("Background (psens wind on)"));
    ui->actionBackground2->setText(tr("Background (psens wind off)"));
    ui->actionWindOn->setText(tr("Wind on (psens)"));
    ui->actionWindOff->setText(tr("Wind off (psens)"));

    ui->actionBackground_3->setText(tr("Background (refrence wind on)"));
    ui->actionBackground_4->setText(tr("Background (refrence wind off)"));
    ui->actionWindOn2->setText(tr("Wind on (refrence)"));
    ui->actionWindOff2->setText(tr("Wind off (refrence"));

    ui->actionHistogram->setText(tr("Histogram"));
    ui->actionHistogram->setShortcut(QKeySequence(tr("Ctrl+H")));
    ui->actionLight_Ratio->setText(tr("Light ratio"));
    ui->actionLight_Ratio->setShortcut(QKeySequence(tr("Ctrl+Alt+L")));
    ui->actionPressure_View->setText(tr("Pressure view"));
    ui->actionSectional_View->setText(tr("Sectional view"));
    ui->actionMake_Field_P_Binary_Paint->setText(tr("Make field P binary paint"));

    ui->actionQPED->setText(tr("QPED"));
    ui->actionQPED_Massive->setText(tr("QPED massive"));
    ui->actionRevise_Cross_Markers->setText(tr("Revise cross markers"));
    ui->actionRevise_Pit_Markers->setText(tr("Revise pit markers"));
    ui->actionRevise_Bump_Markers->setText(tr("Revise bump markers"));

    ui->actionOpen_Project_2->setText(tr("Open Project"));
    ui->actionCalibration->setText(tr("Calibration"));
    ui->actionCalculation->setText(tr("Calculation"));
    ui->actionClose_Project_2->setText(tr("Close project"));

    ui->menuLanguage->setTitle(tr("Language"));
    ui->actionChinese->setText(tr("Chinese"));
    ui->actionEnglish->setText(tr("English"));
    ui->actionContents->setText(tr("Contents"));
    ui->actionContents->setShortcut(QKeySequence(tr("Alt+H")));
    ui->actionAbout->setText(tr("About"));
    ui->actionWebsite->setText(tr("Website"));
}


void MainWindow::updateStatus(const QString &msg) {
    ui->statusBar->showMessage(msg);

    QString logHeader = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    mListLog->addItem(logHeader + " log >> " + msg);
    mListLog->scrollToBottom();
}

void MainWindow::updatePosPixel(const QPointF &pos) {
    QString statusPosPixel = QString("(") + QString::fromStdString(to_string((int) pos.x())) +
                             QString(", ") + QString::fromStdString(to_string((int) pos.y())) + QString(")");;

    const cv::Mat &curImg = WorkImgManager::GetInstance()->getCurImg();
    if (!curImg.empty()) {
        int x = (int) pos.x();
        int y = (int) pos.y();
        if (x < curImg.cols && x >= 0 && y < curImg.rows && y >= 0) {
            if (curImg.depth() == CV_16U) {
                statusPosPixel += QString(" pix: ") + QString::number(curImg.at<ushort>(y, x));
            } else if (curImg.depth() == CV_32F) {
                statusPosPixel += QString(" pix: ") + QString::number(curImg.at<float>(y, x));
            } else if (curImg.depth() == CV_8U) {
                statusPosPixel += QString(" pix: ") + QString::number(curImg.at<uchar>(y, x));
            } else {
                assert(false);
            }
        }
    }

    mLabelMousePos->setText(statusPosPixel);
}

void MainWindow::resetDocument() {


    WorkImgManager::GetInstance()->reset();
    mTreeProperty->clear();
    GraphicManager::GetInstance()->DestroyAllGraphics();
    mViewDisp->reset();

    if (mCurrentDocument) {
        delete mCurrentDocument;
        mCurrentDocument = nullptr;
    }
}

void MainWindow::fileNew() {


    if (!okToContinue()) return;

    resetDocument();

    ui->statusBar->showMessage(QString(tr("Create a new file or project.")));
}

void MainWindow::fileExport() {


    QFileDialog exportDialog(this, QString("Export"));
    exportDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    exportDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Images(*.png)") << tr("Images(*.bmp)") << tr("Images(*.jpg)");
    exportDialog.setNameFilters(fileType);

    if (exportDialog.exec() == QDialog::Accepted) {
        QString filePath = exportDialog.selectedFiles()[0];

        if (filePath.isEmpty() || WorkImgManager::GetInstance()->getCurImg().empty()) return;

        QString selectedFilter = exportDialog.selectedNameFilter();
        int suffixIndex = selectedFilter.lastIndexOf(QString("."));
        QString suffix = selectedFilter.mid(suffixIndex, selectedFilter.length() - suffixIndex - 1);
        if (!filePath.endsWith(suffix)) {
            filePath += suffix;
        }

        mViewDisp->saveImage(filePath);

        LOG_CLIENT(tr("Completed file export operation. File name:") + filePath);
    }
}

void MainWindow::fileExportView() {


    QFileDialog exportDialog(this, QString(tr("Export view")));
    exportDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    exportDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Images(*.png)") << tr("Images(*.bmp)") << tr("Images(*.jpg)");
    exportDialog.setNameFilters(fileType);

    if (exportDialog.exec() == QDialog::Accepted) {
        QString filePath = exportDialog.selectedFiles()[0];

        if (filePath.isEmpty() || WorkImgManager::GetInstance()->getCurImg().empty()) return;

        QString selectedFilter = exportDialog.selectedNameFilter();
        int suffixIndex = selectedFilter.lastIndexOf(QString("."));
        QString suffix = selectedFilter.mid(suffixIndex, selectedFilter.length() - suffixIndex - 1);
        if (!filePath.endsWith(suffix)) {
            filePath += suffix;
        }

        mViewDisp->saveViewImage(filePath);

        LOG_CLIENT(tr("Completed export view operation. File name: ") + filePath);
    }
}

void MainWindow::fileOpen() {

    QFileDialog fileDialog(this, QString(tr("Open file")));
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileFilters;
    fileFilters << tr("Image files (*.bmp *.jpg *.tif)")
                << tr("Any files (*)");
    fileDialog.setNameFilters(fileFilters);
    if (fileDialog.exec()) {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        auto filePath = fileDialog.selectedFiles()[0];
        fileLoad(filePath);

        LOG_CLIENT(tr("Completed file open operation. File name: ") + filePath);
    }
}

void MainWindow::fileClose() {
    fileNew();

    LOG_CLIENT(tr("Close file."));
}

bool MainWindow::fileLoad(const QString &fileName) {


    if (!fileName.isEmpty()) {
        setCurrentFile(fileName);
    } else {
        return false;
    }

    Img_io io;
    bool bRead = io.readImg(cv::String(fileName.toLocal8Bit().data()));

    if (bRead) {
        setDocumentModified(true);

        WorkImgManager::GetInstance()->setCurImg(io.getSrcImg());
    } else {
        LOG_CLIENT(QString(tr("Warning: Read image error!")));
    }

    updateTreeProperty();

    return bRead;
}

void MainWindow::openRecentFile() {


    if (okToContinue()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action) {
            GraphicManager::GetInstance()->DestroyAllGraphics();

            LOG_CLIENT(tr("Open recent file. File name: ") + action->data().toString());

            fileLoad(action->data().toString());
        }
    }
}


vector<MarkerPoint> MainWindow::readMarkerOutside(QString title) {


    vector<MarkerPoint> markerPoints;

    QFileDialog readMarkerAsDialog(this, QString(title));
    readMarkerAsDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    readMarkerAsDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Marker file (*.marker)");
    readMarkerAsDialog.setNameFilters(fileType);

    if (readMarkerAsDialog.exec() == QDialog::Accepted) {
        QString filePath = readMarkerAsDialog.selectedFiles()[0];
        if (filePath.endsWith(".marker")) {
            markerPoints = JsonProcess::readMarkerFromFile(filePath);
        }

        LOG_CLIENT(tr("Open marker points file. File name: ") + filePath);
    }

    return markerPoints;
}

void MainWindow::paintCalibrationNewProject() {


    PspNsManager::PspDocPaintCalibration *document = new PspDocPaintCalibration();
    PagePaintCalibrationNewProject pagePaintCalibrationNewProject(document, nullptr);
    if (pagePaintCalibrationNewProject.exec() == QDialog::Accepted) {
        mCurrentDocument = document;
    } else {
        delete document;
        document = nullptr;
    }
}

void MainWindow::paintCalibrationViewProject() {
    if (PspNsManager::PspDocPaintCalibration *document = dynamic_cast<PspDocPaintCalibration *>(mCurrentDocument)) {
        Json j;
        PspNsManager::to_json(j, *document);

        if (!j.is_null()) {
            mViewDisp->setDocumentInfo(
                    QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore)));

            LOG_CLIENT(tr("View paint calibration project."));
        }
    }
}

void MainWindow::paintCalibrationSaveProject() {
    if (!mCurrentDocument) {
        return;
    }

    Json j;
    if (PspDocPaintCalibration *document = dynamic_cast<PspDocPaintCalibration *>(mCurrentDocument)) {
        PspNsManager::to_json(j, *document);
    } else {
        return;
    }

    if (j.is_null()) {
        return;
    }
    QString projectContent = QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));

    QFileDialog saveProjectDialog(this, QString(tr("Save paint calibration project")));
    saveProjectDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveProjectDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QString pcFileSuffix = ".pcprj";

    QStringList fileType;
    fileType << tr("Paint calibration project file (*.pcprj)");
    saveProjectDialog.setNameFilters(fileType);

    if (saveProjectDialog.exec() == QDialog::Accepted) {
        QString filePath = saveProjectDialog.selectedFiles()[0];
        if (filePath.endsWith(pcFileSuffix))
            filePath.replace(pcFileSuffix, "");

        QFile file(filePath + pcFileSuffix);
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            file.write(projectContent.toLocal8Bit());

            file.close();
        } else {
            return;
        }

        LOG_CLIENT(tr("Save paint calibration project. File name: ") + filePath + pcFileSuffix);
    }
}

void MainWindow::paintCalibrationOpenProject() {
    if (mCurrentDocument) {
        delete mCurrentDocument;
        mCurrentDocument = nullptr;
    }

    resetDocument();

    QFileDialog fileDialog(this, QString(tr("Open paint calibration project")));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileFilters;
    fileFilters << tr("Paint calibration project files (*.pcprj)");
    fileDialog.setNameFilters(fileFilters);

    QString pcFileSuffix = ".pcprj";

    if (fileDialog.exec() == QDialog::Accepted) {
        QString filePath = fileDialog.selectedFiles()[0];

        if (!filePath.endsWith(pcFileSuffix)) return;

        QFile fileProject(filePath);
        if (!fileProject.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        Json j = Json::parse(QString(fileProject.readAll()).toUtf8());
        fileProject.close();

        if (j.is_null()) {
            return;
        }

        PspDocPaintCalibration *document = new PspDocPaintCalibration();
        from_json(j, *document);
        mCurrentDocument = document;

        LOG_CLIENT(tr("Open project. Project file name: ") + filePath);
    }
}

void MainWindow::paintCalibrationDataCollection() {
    if (PspNsManager::PspDocPaintCalibration *document = dynamic_cast<PspNsManager::PspDocPaintCalibration *>(mCurrentDocument)) {
        PagePaintCalibrationDataCollection pagePaintCalibrationDataCollection(document);
        if (pagePaintCalibrationDataCollection.exec() == QDialog::Accepted) {

        }
    }
}

void MainWindow::savePaintCalibrationRange() {

    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorPaintCalibrationRange);

    LOG_CLIENT(tr("Set paint calibration range."));
}

void MainWindow::savePaintCalibrationParameter() {
    DialogPaintCalibrationSaveParameter dialogPaintCalibrationSaveParameter;

    if (dialogPaintCalibrationSaveParameter.exec() == QDialog::Accepted) {
        dialogPaintCalibrationSaveParameter.SaveParameter();

        LOG_CLIENT(tr("Save paint calibration parameter."));
    }
}

void MainWindow::paintCalibration() {
    PagePaintCalibration pagePaintCalibration;
    if (pagePaintCalibration.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Paint calibration operation."));
    }
}

void MainWindow::paintCalibrationSave(const QString &msg) {
    if (msg.isEmpty()) return;

    Json j = Json::parse(msg.toUtf8());
    if (PspDocPaintCalibration *document = dynamic_cast<PspDocPaintCalibration *>(mCurrentDocument)) {
        PspTestFeaturesPaintCalibration features = document->GetTestFeatures();

        if (features.mTestTypeFormula == PspNsBase::eTypeFormula::eQuadratic) {
            for (int i = 0; i < 9; i++) {
                std::string key = QString::number(i).toStdString();
                if (j.contains(key)) {
                    features.mCalibrationCoefficient.push_back(j[key]);
                }
            }
        }

        document->SetTestFeatures(features);
    }

    PagePaintCalibrationSave pagePaintCalibrationSave(msg, nullptr,
                                                      dynamic_cast<PspDocPaintCalibration *>(mCurrentDocument));
    if (pagePaintCalibrationSave.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Save paint calibration results."));
    }
}

void MainWindow::paintCalibraionRangeFromGraphicManager() {
    if (GraphicManager::GetInstance()->GetGraphics().size() > 0) {
        if (PspDocPaintCalibration *document = dynamic_cast<PspDocPaintCalibration *>(mCurrentDocument)) {
            if (GraphicRect *rect = dynamic_cast<GraphicRect *>(GraphicManager::GetInstance()->GetGraphics().back())) {
                document->SetRoi(*rect);
            }
        }
    }
}

void MainWindow::readMarkerFile() {


    vector<MarkerPoint> markerPoints = readMarkerOutside();
    if (!markerPoints.empty()) {
        WorkImgManager::GetInstance()->setMarkerPoints(markerPoints);

        mViewDisp->UpdateView();
    }
}
void MainWindow::saveMarkerFile() {


    QFileDialog saveMarkerAsDialog(this, QString(tr("Save markers")));
    saveMarkerAsDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveMarkerAsDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Marker file (*.marker)");
    saveMarkerAsDialog.setNameFilters(fileType);

    if (saveMarkerAsDialog.exec() == QDialog::Accepted) {
        QString filePath = saveMarkerAsDialog.selectedFiles()[0];
        if (filePath.endsWith(".marker"))
            filePath.replace(".marker", "");

        Json j;
        for (auto marker: WorkImgManager::GetInstance()->getMarkerPoints()) {
            Json jTmp;
            to_json(jTmp, marker);
            j.push_back(jTmp);
        }
        JsonProcess::writeJsonFile(filePath + ".marker", j);

        LOG_CLIENT(tr("Save marker points. File name: ") + filePath + ".marker");
    }
}

bool MainWindow::fileSave() {


    QFileDialog saveImageDialog(this, QString(tr("Save image")));
    saveImageDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveImageDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Image file (*.tif *.tiff)");
    saveImageDialog.setNameFilters(fileType);

    if (saveImageDialog.exec() == QDialog::Accepted) {
        QString filePath = saveImageDialog.selectedFiles()[0];
        if (filePath.isEmpty() || WorkImgManager::GetInstance()->getCurImg().empty()) return false;
        if (!filePath.endsWith(".tif")) {
            filePath += ".tif";
        }

        cv::imwrite(filePath.toStdString(), WorkImgManager::GetInstance()->getCurImg());

        LOG_CLIENT(tr("Completed file save operation. File name: ") + filePath);

        return true;
    }

    return false;
}

bool MainWindow::fileSaveAs() {
    return fileSave();
}

void MainWindow::viewZoomIn() {


    mViewDisp->centerZoom(1.25);

    LOG_CLIENT(tr("View zoom in."));
}

void MainWindow::viewZoomOut() {

    mViewDisp->centerZoom(0.8);

    LOG_CLIENT(tr("View zoom out."));
}

void MainWindow::viewPreview() {

}

void MainWindow::viewNext() {

}

void MainWindow::viewPseudo() {
    SystemParam::GetInstance()->SetIsShowPseudoColor(!SystemParam::GetInstance()->IsShowPseudoColor());

    mViewDisp->UpdateView();

    if (SystemParam::GetInstance()->IsShowPseudoColor()) {
        LOG_CLIENT(tr("Set pseudo display mode."));
        ui->actionViewPseudo->setText(tr("Switch to Gray"));
        ui->actionPseudoConfig->setEnabled(true);
        QIcon icon = QIcon(":/res-icon/icon-pseudo.png");
        if (!icon.isNull()) {
            ui->actionViewPseudo->setIcon(icon);
        }

    } else {
        LOG_CLIENT(tr("Set grayscale display mode."));
        ui->actionViewPseudo->setText(tr("Switch to Pseudo"));
        ui->actionPseudoConfig->setEnabled(false);
        ui->actionViewPseudo->setIcon(QIcon(":/res-icon/icon-gray.png"));
    }
}

void MainWindow::viewPseudoConfig() {
    cv::Mat curImg = WorkImgManager::GetInstance()->getCurImg();
    if (curImg.empty()) return;
    if (curImg.depth() != CV_16U && curImg.depth() != CV_32F) return;

    if (!SystemParam::GetInstance()->IsShowPseudoColor()) return;

    PagePseudoConfig pagePseudoConfig;
    if (pagePseudoConfig.exec() == QDialog::Accepted) {
        QImage img = pagePseudoConfig.GetResultImage().copy();
        mViewDisp->setDrawImage(img);
        mViewDisp->setMarkerPoints(WorkImgManager::GetInstance()->getMarkerPoints());

        LOG_CLIENT(tr("Set pseudo display range."));
    }
}

void MainWindow::viewGray() {
    SystemParam::GetInstance()->SetIsShowPseudoColor(false);

    mViewDisp->UpdateView();

    LOG_CLIENT(tr("Set grayscale display mode."));
}

void MainWindow::viewLengthMeasure() {
    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorLengthMeasure);

    LOG_CLIENT(tr("Length measurement."));
}

void MainWindow::editFilter() {


    PspEditFilter filter;
    filter.exec();

    LOG_CLIENT(tr("Filter image."));
}

void MainWindow::editTransform() {


    PspEditTransform transform;
    transform.exec();
}


void MainWindow::configSystemParam() {


    SystemConfigDialog systemParamDlg;
    if (systemParamDlg.exec() == QDialog::Accepted) {
        SystemParam::GetInstance()->SetBackgroundColor(systemParamDlg.getBgColor());
        mViewDisp->updateBgColor();

        mViewDisp->UpdateView();

        LOG_CLIENT(tr("Config system parameters."));
    }
}

void MainWindow::editRotate() {


    cv::Mat rotatedMat;
    PspNsAlgo::rotate(WorkImgManager::GetInstance()->getCurImg(), rotatedMat, cv::ROTATE_90_CLOCKWISE);
    WorkImgManager::GetInstance()->setCurImg(rotatedMat);

    update();
    LOG_CLIENT(QString(tr("display rotated image.")));

}

void MainWindow::editThreshold() {


    if (WorkImgManager::GetInstance()->getCurImg().empty()) return;

    PageThreshold pageThreshold(WorkImgManager::GetInstance()->getCurImg());
    if (pageThreshold.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Image threshold segmentation."));
    }
}

void MainWindow::editDilate() {
    if (WorkImgManager::GetInstance()->getCurImg().empty()) return;

    cv::Mat src, dilation_dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int dilation_size = 2;

    int dilation_type = 0;
    cv::Mat element = getStructuringElement(dilation_type,
                                            cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                            cv::Point(dilation_size, dilation_size));

    dilate(src, dilation_dst, element);

    WorkImgManager::GetInstance()->setCurImg(dilation_dst);

    LOG_CLIENT(tr("Image dilation."));
}

void MainWindow::editThin() {
    if (WorkImgManager::GetInstance()->getCurImg().empty()) return;

    cv::Mat src, erosion_dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int erosion_size = 2;

    int erosion_type = 0;
    cv::Mat element = getStructuringElement(erosion_type,
                                            cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                            cv::Point(erosion_size, erosion_size));

    erode(src, erosion_dst, element);

    WorkImgManager::GetInstance()->setCurImg(erosion_dst);

    LOG_CLIENT(tr("Image thinning."));
}

void MainWindow::editEdgeCanny() {

    cv::Mat gray;

    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    double lowThreshold, highThreshold;
    calcOstuThreshold(gray, lowThreshold, highThreshold);

    Canny(gray, gray, lowThreshold * 0.5, highThreshold * 0.5);

    WorkImgManager::GetInstance()->setCurImg(gray);

    LOG_CLIENT(tr("Canny edge detection on image."));
}

void MainWindow::editEdgeSobel() {
    cv::Mat src = WorkImgManager::GetInstance()->getCurImg();

    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y, dst;

    cv::Sobel(src, grad_x, src.depth(), 1, 0, 3, 1, 1, cv::BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);

    cv::Sobel(src, grad_y, src.depth(), 0, 1, 3, 1, 1, cv::BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);

    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(tr("Sobel edge detection on image."));
}

void MainWindow::editEdgeRobert() {
}

void MainWindow::editEdgePrewitt() {
}

void MainWindow::editEdgeLaplaccian() {
    cv::Mat src = WorkImgManager::GetInstance()->getCurImg();
    cv::Mat dst;
    Laplacian(src, dst, src.depth(), 3, 1, 0, cv::BORDER_DEFAULT);
    convertScaleAbs(dst, dst);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(tr("Laplacian edge detection on image."));
}

void MainWindow::editEdgeContour() {
    cv::Mat src = WorkImgManager::GetInstance()->getCurImg();
    cv::Mat gray, ThreshGray, dst;

    gray = convertTo8UC1(src);

    double lowThreshold, highThreshold;
    calcOstuThreshold(gray, lowThreshold, highThreshold);
    double threshold = lowThreshold;

    Canny(gray, gray, threshold, threshold * 2);

    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    findContours(gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            gray.at<uchar>(i, j) = 0;
        }
    }

    int index = 0;
    for (; index >= 0; index = hierarchy[index][0]) {
        cv::drawContours(gray, contours, index, cv::Scalar::all(255));
    }


    WorkImgManager::GetInstance()->setCurImg(gray);

    LOG_CLIENT(tr("Contour detection on image."));
}

void MainWindow::editHistEqualization() {
    cv::Mat dstMat;
    dstMat = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    PspNsAlgo::equalizeHist(dstMat, dstMat);
    updateCurMat(dstMat);
    update();

    LOG_CLIENT(tr("Image histogram equalization."));
}

void MainWindow::roiOpen() {
    LogicRoiIO::ReadRoi();

    LOG_CLIENT(tr("Open ROI file."));
}

void MainWindow::roiSave() {
    LogicRoiIO::SaveRoi();

    LOG_CLIENT(tr("Save ROI file."));
}

void MainWindow::roiByBox() {
    OperatorROIAdd *pOperatorRoiAdd = dynamic_cast<OperatorROIAdd *>(OperatorFactory::CreateOperator(
            OperatorType::eOperatorROISet));
    pOperatorRoiAdd->SetROIType(eROIType::eBox);

    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorROISet);

    LOG_CLIENT(tr("Set rectangular ROI."));
}

void MainWindow::roiByPolygon() {
    OperatorROIAdd *pOperatorRoiAdd = dynamic_cast<OperatorROIAdd *>(OperatorFactory::CreateOperator(
            OperatorType::eOperatorROISet));
    pOperatorRoiAdd->SetROIType(eROIType::ePolygon);

    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorROISet);

    LOG_CLIENT(tr("Set polygon ROI."));
}

void MainWindow::addMarkerManual() {
    OperatorMarkerAdd *pOperatorAdd = dynamic_cast<OperatorMarkerAdd *>(OperatorFactory::CreateOperator(
            OperatorType::eOperatorMarkerAdd));

    Dialog_OperatorMarkerAdd_Size dlgSetMarkerAddSize;
    dlgSetMarkerAddSize.SetBoxSize(pOperatorAdd->GetBoxSize());

    if (dlgSetMarkerAddSize.exec() == QDialog::Accepted) {
        pOperatorAdd->SetBoxSize(dlgSetMarkerAddSize.GetBoxSize());

        PspViewManager::GetInstance()->UpdateView();
    } else {
        return;
    }

    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorMarkerAdd);

    LOG_CLIENT(tr("Enter manual detection marker mode."));
}

void MainWindow::deleteMarkerManual() {
    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorMarkerDelete);

    LOG_CLIENT(tr("Enter manually delete marker points mode."));
}

void MainWindow::pinMarker() {
    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorMarkerPin);

    LOG_CLIENT(tr("Use the pin method to give a marker point."));
}

void MainWindow::autoMarker() {
    if (WorkImgManager::GetInstance()->getCurImg().empty()) return;

    unsigned int markerRadius = 2;
    Dialog_OperatorMarkerAdd_Size dialogMarkerSize;
    if (dialogMarkerSize.exec() == QDialog::Accepted) {
        markerRadius = dialogMarkerSize.GetBoxSize() / 2;
    } else {
        return;
    }

    vector<cv::RotatedRect> minEllipse;
    vector<QPointF> points;
    cv::Mat tmp = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    PspNsAlgo::fitEllipse(tmp, minEllipse, points, markerRadius);

    if (minEllipse.size() != points.size()) {
        assert(false);
    }

    cv::Mat drawEllipseMat = WorkImgManager::GetInstance()->getCurImg();
    if (SystemParam::GetInstance()->IsDebugMode()) {
        for (auto iEllipse: minEllipse) {
            ellipse(drawEllipseMat, iEllipse, cv::Scalar(255, 0, 0));
        }
    }

    vector<MarkerPoint> markerPoints;
    for (size_t i = 0; i < points.size(); i++) {
        markerPoints.push_back(MarkerPoint(i, points[i], minEllipse[i].boundingRect().width / 2));
    }

    WorkImgManager::GetInstance()->setCurImg(drawEllipseMat);
    setDrawMatAndMarkerPoints(drawEllipseMat, markerPoints);

    LOG_CLIENT(tr("Completed automatic detection of marker points operation."));
}

void MainWindow::referenceMarker() {
    Img_io ioReference = ReadImage(tr("Read reference image"));
    cv::Mat srcTemplate = convertTo8UC1(ioReference.getSrcImg());
    vector<MarkerPoint> referenceMarkerPoints = readMarkerOutside(tr("Read reference markers"));
    if (referenceMarkerPoints.empty()) {
        Log::getInstance()->writeLog(LogLevel::eWarning, tr("The reference marker points is empty. Exit!"));
        return;
    }

    vector<MarkerPoint> matchedMarkerPoints;
    for (auto mp: referenceMarkerPoints) {
        MarkerPoint mpMatch;
        cv::Mat MatchResultImg;
        double searchRadius = 20;
        cv::Mat tmp = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
        if (MatchTemplate(srcTemplate, tmp, mp, searchRadius, mpMatch, -1)) {
            matchedMarkerPoints.push_back(mpMatch);
        }
    }

    setMarkerPoints(matchedMarkerPoints);
    mViewDisp->UpdateView();

    LOG_CLIENT(tr("Detect the current image marker points by reference marker method."));
}

void MainWindow::hideMarker() {
    bool isShowMarker = SystemParam::GetInstance()->IsShowMarkers();
    SystemParam::GetInstance()->SetIsShowMarkers(!isShowMarker);

    if (SystemParam::GetInstance()->IsShowMarkers()) {
        ui->actionHide_Markers->setText(tr("Hide markers"));

        LOG_CLIENT(tr("Show markers"));
    } else {
        ui->actionHide_Markers->setText(tr("Show markers"));

        LOG_CLIENT(tr("Hide markers"));
    }
}

void MainWindow::autoSort() {
    Dialog_AutoSortMakers dialogAutoSortMarkers;

    if (dialogAutoSortMarkers.exec() == QDialog::Accepted) {
        vector<MarkerPoint> markerPoints = WorkImgManager::GetInstance()->getMarkerPoints();

        int methodIndex = dialogAutoSortMarkers.GetMethodIndex();
        if (methodIndex > -1 && methodIndex < 4) {
            SortMarkPoints(markerPoints, (eBaseSortPoints) methodIndex);

            SystemParam::GetInstance()->SetSortPointType((eBaseSortPoints) methodIndex);

            WorkImgManager::GetInstance()->setMarkerPoints(markerPoints);
            updateTreeProperty();

            mViewDisp->setMarkerPoints(WorkImgManager::GetInstance()->getMarkerPoints());
            mViewDisp->UpdateView();
        }

        LOG_CLIENT(tr("Completed automatic sort marker points operation."));
    }
}

void MainWindow::fillupMarkers() {
    WorkImgManager::GetInstance()->fillupMarkers();
    updateDrawMat();

    LOG_CLIENT(tr("Fill up marker points."));
}

void MainWindow::translateMarker() {
    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorMarkerTranslate);
}

void MainWindow::imageAlignment() {
    PageImageAlignment pageImageAlignment;
    if (pageImageAlignment.exec() == QDialog::Accepted) {

    }
}

void MainWindow::imageAddition() {
    PageTwoImageCalculation twoImageCalcilation(nullptr, eTypeTwoImageCalculation::eAddition);
    if (twoImageCalcilation.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Image addition."));
    }
}

void MainWindow::imageSbstraction() {
    PageTwoImageCalculation twoImageCalcilation(nullptr, eTypeTwoImageCalculation::eSubtract);
    if (twoImageCalcilation.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Image subtraction."));
    }
}

void MainWindow::imageDivision() {
    PageTwoImageCalculation twoImageCalcilation(nullptr, eTypeTwoImageCalculation::eDivision);
    if (twoImageCalcilation.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Image division."));
    }
}

void MainWindow::imageAverage() {
    QStringList fileName = QFileDialog::getOpenFileNames(
            this,
            tr("Image average"),
            QString(),
            "Images (*.tif)");

    if (fileName.empty()) return;

    Img_io io;

    cv::Mat average, img;

    for (int i = 0; i < fileName.length(); i++) {
        if (io.readImg(fileName.at(i).toStdString())) {
            img = io.getSrcImg();
        }
        img.convertTo(img, CV_32FC1);

        if (i == 0) {
            average = cv::Mat::zeros(img.size(), CV_32FC1);
        }

        if (img.size() != average.size()) {
            LOG_CLIENT(tr("Warning: Differnt size images in average operation! file name: ") + fileName.at(i));
        } else {
            cv::accumulate(img, average);
        }
    }
    average = average / fileName.length();

    updateCurMat(average);
    LOG_CLIENT(QString(tr("Images average operation completed.")));
}

void MainWindow::sectionalView() {
    OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorSectionalView);
}

void MainWindow::sectionalViewProcess(QPointF start, QPointF end) {
    QPoint s = QPoint(start.rx(), start.ry());
    QPoint e = QPoint(end.rx(), end.ry());

    if (s == e) return;
    if (WorkImgManager::GetInstance()->getCurImg().empty()) return;

    cv::Mat curMat = WorkImgManager::GetInstance()->getCurImg();

    if (s.x() < 0 || s.x() >= curMat.cols || s.y() < 0 || s.y() >= curMat.rows) return;
    if (e.x() < 0 || e.x() >= curMat.cols || e.y() < 0 || e.y() >= curMat.rows) return;

    int minY = min(s.y(), e.y());
    int maxY = max(s.y(), e.y());
    int minX = min(s.x(), e.x());
    int maxX = max(s.x(), e.x());

    vector<double> valueY, valueX;
    eSectionalViewType type = eSectionalViewType::eVert;
    if (s.x() == e.x()) {
        for (int i = minY; i <= maxY; i++) {
            valueX.push_back(getPixel(curMat, s.x(), i));
        }

        type = eSectionalViewType::eVert;
    } else if (s.y() == e.y()) {
        for (int i = minX; i <= maxX; i++) {
            valueY.push_back(getPixel(curMat, i, s.y()));
        }

        type = eSectionalViewType::eHoriz;
    } else {
        double k = (s.y() - e.y()) / (double) (s.x() - e.x());

        for (int i = minY; i <= maxY; i++) {
            int x = (int) (s.x() + (i - s.y()) / k);
            valueX.push_back(getPixel(curMat, x, i));
        }
        for (int i = minX; i <= maxX; i++) {
            int y = (int) (s.y() + (i - s.x()) * k);
            valueY.push_back(getPixel(curMat, i, y));
        }

        type = eSectionalViewType::eBilateral;
    }

    PageSectionalView sectionalView(type, minX, maxX, minY, maxY, valueX, valueY);
    sectionalView.exec();

    LOG_CLIENT(tr("Display sectional view."));
}

void MainWindow::histogram() {
    if (WorkImgManager::GetInstance()->getCurImg().empty()) return;
    if (WorkImgManager::GetInstance()->getCurImg().depth() == CV_32F) return;

    PageHistogram pageHistogram(WorkImgManager::GetInstance()->getCurImg());
    if (pageHistogram.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("View histogram."));
    }
}

void MainWindow::frequencyDomainView() {
    cv::Mat src, dst, I;
    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    if (src.empty()) return;
    src.copyTo(I);

    cv::Mat padded;
    int m = cv::getOptimalDFTSize(I.rows);
    int n = cv::getOptimalDFTSize(I.cols);
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);

    split(complexI, planes);
    magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magI = planes[0];
    magI += cv::Scalar::all(1);
    log(magI, magI);

    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    int cx = magI.cols / 2;
    int cy = magI.rows / 2;
    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    normalize(magI, magI, 0, 1, cv::NORM_MINMAX);
    magI = convertTo8UC1(magI);

    QImage img = convertMatToImage(magI);

    PageFrequency pageFrequency(img);
    if (pageFrequency.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("View frequency domain diagram."));
    }
}

void MainWindow::setImgAsBackgound() {


    mImgManager->setBackgroundImg(WorkImgManager::GetInstance()->getCurImg());
    LOG_CLIENT(QString(tr("Set image as background image.")));
}

void MainWindow::setImgAsReference() {


    mImgManager->setReferenceImg(WorkImgManager::GetInstance()->getCurImg());
    LOG_CLIENT(QString(tr("Set image as reference image.")));
}

void MainWindow::setImgAsPsp() {


    mImgManager->setPspImg(WorkImgManager::GetInstance()->getCurImg());
    LOG_CLIENT(QString(tr("Set image as psp image.")));
}


void MainWindow::clearTreeProperty() {
    QTreeWidgetItemIterator it(mTreeProperty);

    while (*it) {
        delete *it;

        ++it;
    }
    mTreeProperty->clear();
}

void MainWindow::displayBackgroundImg() {


    updateCurMat(mImgManager->getBackgroundImg());

    LOG_CLIENT(QString(tr("display background image.")));
}

void MainWindow::displayReferenceImg() {


    updateCurMat(mImgManager->getReferenceImg());

    LOG_CLIENT(QString(tr("display reference image.")));
}

void MainWindow::displayPspImg() {


    updateCurMat(mImgManager->getPspImg());

    LOG_CLIENT(QString(tr("display psp image.")));
}

void MainWindow::MorphologyOpening() {
    cv::Mat src, dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int morph_elem = 2;
    int morph_size = 2;
    int morph_operator = 0;

    int operation = morph_operator + 2;
    cv::Mat element = getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
                                            cv::Point(morph_size, morph_size));
    morphologyEx(src, dst, operation, element);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(QString(tr("Morphological opening transformation.")));
}

void MainWindow::MorphologyClosing() {
    cv::Mat src, dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int morph_elem = 2;
    int morph_size = 2;
    int morph_operator = 1;

    int operation = morph_operator + 2;
    cv::Mat element = getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
                                            cv::Point(morph_size, morph_size));
    morphologyEx(src, dst, operation, element);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(QString(tr("Morphological closing transformation.")));
}

void MainWindow::MorphologyMorphologicalGradient() {
    cv::Mat src, dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int morph_elem = 2;
    int morph_size = 2;
    int morph_operator = 2;

    int operation = morph_operator + 2;
    cv::Mat element = getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
                                            cv::Point(morph_size, morph_size));
    morphologyEx(src, dst, operation, element);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(QString(tr("Morphological gradient transformation.")));
}

void MainWindow::MorphologyTop_Hat() {
    cv::Mat src, dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int morph_elem = 2;
    int morph_size = 2;
    int morph_operator = 3;

    int operation = morph_operator + 2;
    cv::Mat element = getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
                                            cv::Point(morph_size, morph_size));
    morphologyEx(src, dst, operation, element);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(QString(tr("Morphological top hat transformation.")));
}

void MainWindow::MorphologyBlack_Hat() {
    cv::Mat src, dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int morph_elem = 2;
    int morph_size = 2;
    int morph_operator = 4;

    int operation = morph_operator + 2;
    cv::Mat element = getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
                                            cv::Point(morph_size, morph_size));
    morphologyEx(src, dst, operation, element);

    WorkImgManager::GetInstance()->setCurImg(dst);

    LOG_CLIENT(QString(tr("Morphological black hat transformation.")));
}

Img_io MainWindow::ReadImage(QString title) {
    Img_io io;

    QFileDialog fileDialog(this, QString(title));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileFilters;
    fileFilters << tr("Image files (*.bmp *.jpg *.tif)")
                << tr("Any files (*)");
    fileDialog.setNameFilters(fileFilters);
    if (fileDialog.exec() == QDialog::Accepted) {
        QString filePath = fileDialog.selectedFiles()[0];

        io.readImg(cv::String(filePath.toLocal8Bit().data()));
    }

    return io;
}

void MainWindow::aboutSoftware() {
    QString text = tr("PspTool Alpha\n\n");
    text += tr("Version 1.0.0.0\n\n");
    text += tr("https://xxxx.xxx\n\n");
    text += tr("Copyright (C) 20xx-20xx xxx Company\n");

    QMessageBox::about(this, tr("About"), text);
}

void MainWindow::openWebsite() {
    QDesktopServices::openUrl(QUrl(QString("https://xxxx.xxx")));
}

void MainWindow::helpManual() {
    QString assistantApp = QCoreApplication::applicationDirPath() + "/assistant";
#ifdef PLATFORM_WINDOWS
    assistantApp += ".exe";
#endif

    QStringList arguments;
    arguments << "-collectionfile" << "psp.qhc";

    QProcess *myProcess = new QProcess();
    myProcess->start(assistantApp, arguments);
}

void MainWindow::newProjectSinglComponent() {
    PageProjectNew projectNew(ProjectType::eSingleComponent);

    if (projectNew.exec() == QDialog::Accepted) {
        mCurrentDocument = projectNew.GetDocument();

        mViewDisp->UpdateView();

        LOG_CLIENT(tr("Create single component document."));
    }
}

void MainWindow::newProjectDualComponent() {
    PageProjectNew projectNew(ProjectType::eDualComponent);

    if (projectNew.exec() == QDialog::Accepted) {
        mCurrentDocument = projectNew.GetDocument();

        mViewDisp->UpdateView();

        LOG_CLIENT(tr("Create dual component document."));
    }
}

void MainWindow::projectFeatures() {
    if (!mCurrentDocument) {
        QMessageBox msgBox;
        msgBox.setText(tr("You must create a new project firsrtly."));
        msgBox.exec();

        return;
    }

    PageProjectFeatures pageProjectFeatures(nullptr, mCurrentDocument);
    if (pageProjectFeatures.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Config test features."));
    }
}

void MainWindow::openProject() {
    if (mCurrentDocument) {
        delete mCurrentDocument;
        mCurrentDocument = nullptr;
    }

    resetDocument();

    QFileDialog fileDialog(this, QString(tr("Open project")));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileFilters;
    fileFilters << tr("Project files (*.prf)");
    fileDialog.setNameFilters(fileFilters);

    if (fileDialog.exec() == QDialog::Accepted) {
        QString filePath = fileDialog.selectedFiles()[0];

        if (!filePath.endsWith(".prf")) return;

        QFile fileProject(filePath);
        if (!fileProject.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        Json j = Json::parse(QString(fileProject.readAll()).toUtf8());

        fileProject.close();

        if (mCurrentDocument) {
            delete mCurrentDocument;
            mCurrentDocument = nullptr;
        }

        if (j.contains("type") && j["type"] == "Single component document") {
            mCurrentDocument = new PspDocumentSingleComponent();
            PspNsManager::from_json(j, (PspDocumentSingleComponent &) *mCurrentDocument);
        } else if (j.contains("type") && j["type"] == "Two component document") {
            mCurrentDocument = new PspDocumentTwoComponent();
            PspNsManager::from_json(j, (PspDocumentTwoComponent &) (*mCurrentDocument));
        } else {
            return;
        }

        LOG_CLIENT(tr("Open project. Project file name: ") + filePath);
    }
}

void MainWindow::saveProject() {
    if (!mCurrentDocument) {
        return;
    }

    Json j;
    if (PspDocumentTwoComponent *document = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        PspNsManager::to_json(j, *document);
    } else if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        PspNsManager::to_json(j, *document);
    }

    if (j.is_null()) {
        return;
    }
    QString projectContent = QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));

    QFileDialog saveProjectDialog(this, QString(tr("Save project")));
    saveProjectDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveProjectDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Project file (*.prf)");
    saveProjectDialog.setNameFilters(fileType);

    if (saveProjectDialog.exec() == QDialog::Accepted) {
        QString filePath = saveProjectDialog.selectedFiles()[0];
        if (filePath.endsWith(".prf"))
            filePath.replace(".prf", "");

        QFile file(filePath + ".prf");
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            file.write(projectContent.toLocal8Bit());

            file.close();
        } else {
            return;
        }

        LOG_CLIENT(tr("Save project. File name: ") + filePath);
    }
}

void MainWindow::saveProjectResults() {
    if (!mCurrentDocument) {
        return;
    }

    if (PspDocumentTwoComponent *document = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        if (document->GetLightRatioImg().empty()) return;
    } else if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        if (document->GetLightRatioImg().empty()) return;
    }

    QFileDialog saveProjectResultsDlg(this, tr("Save project result"));
    saveProjectResultsDlg.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveProjectResultsDlg.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("Project file (*.prf)");
    saveProjectResultsDlg.setNameFilters(fileType);

    if (saveProjectResultsDlg.exec() == QDialog::Accepted) {
        QString dirPath = saveProjectResultsDlg.selectedFiles()[0];
        if (dirPath.endsWith(".prf")) dirPath.replace(".prf", "");

        QDir dir;
        dir.mkpath(dirPath);

        QFileInfo info(dirPath);
        QString fileBaseName = info.baseName();
        QString prjPath = dirPath + "/" + fileBaseName + ".prf";
        QString lightRatioPath = dirPath + "/LightRatio.tif";
        QString pressuewViewPath = dirPath + "/Pressure.tif";

        Json j;
        if (PspDocumentTwoComponent *document = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
            document->SetPathLigthRationImg(lightRatioPath);
            document->SetPathPressureViewImg(pressuewViewPath);

            PspNsManager::to_json(j, *document);

            if (!document->GetLightRatioImg().empty()) {
                cv::imwrite(lightRatioPath.toStdString(), document->GetLightRatioImg());
            }
            if (!document->GetPressureImg().empty()) {
                cv::imwrite(pressuewViewPath.toStdString(), document->GetPressureImg());
            }
        } else if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
            document->SetPathLigthRationImg(lightRatioPath);
            document->SetPathPressureViewImg(pressuewViewPath);

            PspNsManager::to_json(j, *document);

            if (!document->GetLightRatioImg().empty()) {
                cv::imwrite(lightRatioPath.toStdString(), document->GetLightRatioImg());
            }
            if (!document->GetPressureImg().empty()) {
                cv::imwrite(pressuewViewPath.toStdString(), document->GetPressureImg());
            }
        }

        if (j.is_null()) {
            return;
        }
        QString projectContent = QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));

        QFile file(prjPath);
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            file.write(projectContent.toLocal8Bit());

            file.close();
        } else {
            return;
        }

        LOG_CLIENT(tr("Save project results. Path name: ") + prjPath);
    }

}

void MainWindow::closeProject() {
    if (mCurrentDocument) {
        delete mCurrentDocument;
        mCurrentDocument = nullptr;
    }

    resetDocument();

    LOG_CLIENT(tr("Close project."));
}

void MainWindow::processProject() {
    if (!mCurrentDocument) {
        QMessageBox msgBox;
        msgBox.setText(tr("Warning: You must create a new project firsrtly."));
        msgBox.exec();

        return;
    }

    PageProjectProcess pageProjectProcess(nullptr, mCurrentDocument);
    if (pageProjectProcess.exec() == QDialog::Accepted) {
        LOG_CLIENT(tr("Completed project processing operation."));
    }
}

void MainWindow::manageProject() {


    ManageProject dlg(this);
    dlg.exec();
}

void MainWindow::viewProject() {
    Json j;

    if (PspDocumentTwoComponent *document = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        PspNsManager::to_json(j, *document);
    } else if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        PspNsManager::to_json(j, *document);
    }

    if (!j.is_null()) {
        mViewDisp->setDocumentInfo(
                QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore)));

        LOG_CLIENT(tr("View project."));
    }
}

void MainWindow::lightRatio() {
    if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        if (!document->GetLightRatioImg().empty()) {
            WorkImgManager::GetInstance()->setCurImg(document->GetLightRatioImg());
        } else if (!document->GetPathLigthRationImg().isEmpty()) {
            cv::Mat ligthRatioImg = cv::imread(document->GetPathLigthRationImg().toStdString(), cv::IMREAD_ANYDEPTH);
            WorkImgManager::GetInstance()->setCurImg(ligthRatioImg);
        }

        LOG_CLIENT(tr("Display light ratio image."));
    }
}

void MainWindow::pressureView() {
    if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        if (!document->GetPressureImg().empty()) {
            WorkImgManager::GetInstance()->setCurImg(document->GetPressureImg());
        } else if (!document->GetPathPressureViewImg().isEmpty()) {
            cv::Mat pressureViewImg = cv::imread(document->GetPathPressureViewImg().toStdString(), cv::IMREAD_ANYDEPTH);
            WorkImgManager::GetInstance()->setCurImg(pressureViewImg);
        }

        LOG_CLIENT(tr("Display pressure view image."));
    }
}

void MainWindow::pressureCoefficientView() {
    if (PspDocumentSingleComponent *document = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        if (!document->GetPressureCoefficientImg().empty()) {
            WorkImgManager::GetInstance()->setCurImg(document->GetPressureCoefficientImg());
        } else if (!document->GetPathPressureCoefficientImg().isEmpty()) {
            cv::Mat pressureCoefficientViewImg = cv::imread(document->GetPathPressureCoefficientImg().toStdString(),
                                                            cv::IMREAD_ANYDEPTH);
            WorkImgManager::GetInstance()->setCurImg(pressureCoefficientViewImg);
        }

        LOG_CLIENT(tr("Display pressure coefficient view image."));
    }
}

void MainWindow::projectCurrentImageWindOn() {
    if (!mCurrentDocument) return;

    if (PspDocumentSingleComponent *pSingleComponentDoc = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pSingleComponentDoc->GetWindOnImg());

        LOG_CLIENT(tr("Display image in psens wind on status."));
    }
}

void MainWindow::projectCurrentImageWindOff() {
    if (!mCurrentDocument) return;

    if (PspDocumentSingleComponent *pSingleComponentDoc = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pSingleComponentDoc->GetWindOffImg());

        LOG_CLIENT(tr("Display image in psens wind off status."));
    }
}

void MainWindow::projectCurrentImageWindOn2() {
    if (!mCurrentDocument) return;

    if (PspDocumentTwoComponent *pTwoComponentDoc = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pTwoComponentDoc->GetWindOn2Img());

        LOG_CLIENT(tr("Display image in reference wind on status."));
    }
}

void MainWindow::projectCurrentImageWindOff2() {
    if (!mCurrentDocument) return;

    if (PspDocumentTwoComponent *pTwoComponentDoc = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pTwoComponentDoc->GetWindOff2Img());

        LOG_CLIENT(tr("Display image in reference wind off status."));
    }
}

void MainWindow::projectCurrentImageBackground() {
    if (!mCurrentDocument) return;

    if (PspDocumentSingleComponent *pSingleComponentDoc = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pSingleComponentDoc->GetBackgroundImg());

        LOG_CLIENT(tr("Display background image in psens wind on status."));
    }
}

void MainWindow::projectCurrentImageBackground2() {
    if (!mCurrentDocument) return;

    if (PspDocumentSingleComponent *pSingleComponentDoc = dynamic_cast<PspDocumentSingleComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pSingleComponentDoc->GetBackground2Img());

        LOG_CLIENT(tr("Display background image in psens wind off status."));
    }
}

void MainWindow::projectCurrentImageBackground3() {
    if (!mCurrentDocument) return;

    if (PspDocumentTwoComponent *pDualComponentDoc = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pDualComponentDoc->GetBackground3Img());

        LOG_CLIENT(tr("Display background image in reference wind on status."));
    }
}

void MainWindow::projectCurrentImageBackground4() {
    if (!mCurrentDocument) return;

    if (PspDocumentTwoComponent *pDualComponentDoc = dynamic_cast<PspDocumentTwoComponent *>(mCurrentDocument)) {
        WorkImgManager::GetInstance()->setCurImg(pDualComponentDoc->GetBackground4Img());

        LOG_CLIENT(tr("Display background image in reference wind off status."));
    }
}

bool MainWindow::okToContinue() {


    if (mIsDocumentModified) {
        int ret = QMessageBox::warning(this, tr("Warning"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save it?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (ret == QMessageBox::Yes) {
            return fileSave();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }

    return true;
}

void MainWindow::setDocumentModified(bool modified) {


    mIsDocumentModified = modified;
}

void MainWindow::setCurrentFile(const QString &fileName) {


    mCurFile = fileName;

    if (!mCurFile.isEmpty()) {
        mRecentFiles.removeAll(mCurFile);
        mRecentFiles.prepend(mCurFile);
        updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions() {


    QMutableStringListIterator i(mRecentFiles);

    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaxRecentFileCount; j++) {
        if (j < mRecentFiles.count()) {
            QString text = tr("&%1 %2").arg(j + 1).arg(QFileInfo(mRecentFiles[j]).fileName());
            mRecentFilesAction[j]->setText(text);
            mRecentFilesAction[j]->setData(mRecentFiles[j]);
            mRecentFilesAction[j]->setVisible(true);
        } else {
            mRecentFilesAction[j]->setVisible(false);
        }
    }
}

void MainWindow::updateTreeProperty() {


    clearTreeProperty();

    QTreeWidgetItem *top1 = new QTreeWidgetItem(mTreeProperty);
    top1->setText(0, "info");
    QTreeWidgetItem *top2 = new QTreeWidgetItem(mTreeProperty);
    top2->setText(0, "markers");

    mTreeProperty->addTopLevelItem(top1);
    QTreeWidgetItem *sub_width = new QTreeWidgetItem(top1);
    sub_width->setText(0, "width");
    sub_width->setText(1, QString::number(WorkImgManager::GetInstance()->getCurImg().cols));
    QTreeWidgetItem *sub_height = new QTreeWidgetItem(top1);
    sub_height->setText(0, "height");
    sub_height->setText(1, QString::number(WorkImgManager::GetInstance()->getCurImg().rows));
    top1->addChild(sub_width);
    top1->addChild(sub_height);

    mTreeProperty->addTopLevelItem(top2);
    const vector<MarkerPoint> &markerPoints = WorkImgManager::GetInstance()->getMarkerPoints();
    for (size_t i = 0; i < markerPoints.size(); i++) {
        QTreeWidgetItem *sub_marker = new QTreeWidgetItem(top2);
        sub_marker->setText(0, QString::number(markerPoints[i].getIndex()));
        sub_marker->setText(1,
                            "(" + QString::number(markerPoints[i].x()) + ", " + QString::number(markerPoints[i].y()) +
                            ")");
        top2->addChild(sub_marker);
    }

    mTreeProperty->expandAll();
}

void MainWindow::updateMarkerPoints() {
    mViewDisp->setMarkerPoints(WorkImgManager::GetInstance()->getMarkerPoints());
    update();
}

void MainWindow::updateROI() {
    mViewDisp->setROI(WorkImgManager::GetInstance()->getROI());
    update();
}

void MainWindow::writeSettings() {


    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", mRecentFiles);

    settings.setValue("PspTool/backgroundColor", SystemParam::GetInstance()->GetBackgroundColor());
    settings.setValue("PspTool/viewMode", SystemParam::GetInstance()->IsShowPseudoColor());
}

void MainWindow::readSettings() {


    QSettings settings;

    restoreGeometry(settings.value("geometry").toByteArray());
    mRecentFiles = settings.value("recentFiles").toStringList();

    QVariant varBackgroundColor = settings.value("PspTool/backgroundColor");
    QVariant varViewMode = settings.value("PspTool/viewMode");
    if (!varBackgroundColor.isNull()) {
        SystemParam::GetInstance()->SetBackgroundColor(varBackgroundColor.toInt());
    }
    if (!varViewMode.isNull()) {
        SystemParam::GetInstance()->SetIsShowPseudoColor(varViewMode.toBool());
    }
}

void MainWindow::updateCurMat(const cv::Mat &mat) {
    WorkImgManager::GetInstance()->setCurImg(mat);
    updateDrawMat();
}

QImage MainWindow::convertMatToImage(const cv::Mat &mat) {
    if (mat.depth() == CV_8U && mat.channels() == 3) {
        GraphicColorBar::GetInstance()->SetColorMin(0);
        GraphicColorBar::GetInstance()->SetColorMax(255);
        return QImage((unsigned char *) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
    }

    cv::Mat tmp;

    double minVal, maxVal;
    cv::minMaxLoc(mat, &minVal, &maxVal);

    if (fabs(maxVal - minVal) < DBL_EPSILON) {
        LOG_WARNING(tr("The value of all pixels are equal!"));
        return QImage();
        assert(false);
    }

    GraphicColorBar::GetInstance()->SetColorMin(minVal);
    GraphicColorBar::GetInstance()->SetColorMax(maxVal);

    mat.convertTo(tmp, CV_8UC1, 255.0 / (maxVal - minVal), -255.0 * minVal / (maxVal - minVal));
    cv::cvtColor(tmp, tmp, cv::COLOR_GRAY2RGB);

    return QImage((unsigned char *) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888).copy();
}

void MainWindow::updateDrawMat() {
    const cv::Mat &mat = WorkImgManager::GetInstance()->getCurImg();
    if (mat.empty()) {
        mViewDisp->reset();
        return;
    }

    QImage img = convertMatToImage(mat);
    mViewDisp->setDrawImage(img);
    mViewDisp->setMarkerPoints(WorkImgManager::GetInstance()->getMarkerPoints());
    mViewDisp->UpdateView();

    update();
}

void MainWindow::setMarkerPoints(const vector<QPointF> &points) {
    if (points.empty()) return;

    vector<MarkerPoint> markerPoints;
    int counter = 0;
    for (auto iter = points.begin(); iter != points.end(); ++iter) {
        markerPoints.push_back(MarkerPoint(counter, *iter, 3));
        counter++;
    }

    WorkImgManager::GetInstance()->setMarkerPoints(markerPoints);
}

void MainWindow::setMarkerPoints(const vector<MarkerPoint> &points) {
    if (points.empty()) return;

    WorkImgManager::GetInstance()->setMarkerPoints(points);
}

void MainWindow::setDrawMatAndMarkerPoints(const cv::Mat &mat, const vector<QPointF> &points) {
    updateCurMat(mat);
    setMarkerPoints(points);
}

void MainWindow::setDrawMatAndMarkerPoints(const cv::Mat &mat, const vector<MarkerPoint> &points) {
    updateCurMat(mat);
    setMarkerPoints(points);
}

void MainWindow::closeEvent(QCloseEvent *event) {


    if (mIsDocumentModified) {
        int ret = QMessageBox::warning(this, tr("Warning"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save it?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (ret == QMessageBox::Yes) {
            fileSave();
        } else if (ret == QMessageBox::Cancel) {
            event->ignore();
        } else {
        }
    }

    writeSettings();
}

void MainWindow::paintEvent(QPaintEvent *e) {

}

bool MainWindow::eventFilter(QObject *object, QEvent *e) {
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = (QKeyEvent *) e;

        if (keyEvent->key() == Qt::Key_Escape) {
            OperatorManager::GetInstance()->Finish();
            mViewDisp->UpdateView();
        }
    }

    return true;
}

void MainWindow::emitQuit() {
    emit destroyed();
}
