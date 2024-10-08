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
#pragma once

#include <QMainWindow>
#include <QObject>
#include <QCloseEvent>
#include <QListWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QTranslator>
#include "psp_operator_util.h"
#include "io/io.h"
#include "manager/manager.h"
#include "manager/work_manager.h"
#include "algo/algo.h"
#include "algo/algo_match.h"
#include "algo/algo_2d_features.h"
#include "algo/algo_threshold.h"
#include "algo/algo_transformation.h"
#include "manager/manager_doc.h"
#include "view/view_disp.h"
#include "view/view_display_widget.h"

using namespace PspNsImgIo;
using namespace PspNsManager;
using namespace PspNsAlgo;
using namespace PspNsView;

namespace Ui {
    class MainWindow;
}

class QHBoxLayout;
class QVBoxLayout;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTranslator *GetTranslator() const;
    void translateUi();

protected:
    void closeEvent(QCloseEvent *event);

    void paintEvent(QPaintEvent *e);

    bool eventFilter(QObject *object, QEvent *e);
private:
    void InitLayout();
    void InitLanguage();
    void InitOperator();

    void InitMenu();
    void InitFileMenu();
    void InitViewMenu();
    void InitEditMenu();
    void InitROIMenu();
    void InitPaintCalibration();
    void InitMarkerMenu();
    void InitOptionsMenu();
    void InitProcessingMenu();
    void InitToolsMenu();
    void InitQpedMenu();
    void Init3DDeformationMenu();
    void InitBConvertMenu();
    void InitHelpMenu();
    void InitTestMenu();
    void InitTest2Menu();
    void InitTest3Menu();


    void InitMainToolBar();
    void InitStatusBar();

    void InitView();

    bool okToContinue();
    void setDocumentModified(bool modified = true);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();

    void writeSettings();
    void readSettings();

    void updateCurMat(const cv::Mat &mat);
    void setMarkerPoints(const vector<QPointF> &points);
    void setMarkerPoints(const vector<MarkerPoint> &points);
    void setDrawMatAndMarkerPoints(const cv::Mat &mat, const vector<QPointF> &points);
    void setDrawMatAndMarkerPoints(const cv::Mat &mat, const vector<MarkerPoint> &points);

    void resetDocument();

    vector<MarkerPoint> readMarkerOutside(QString title = tr("Read markers"));


public slots:
    void updateStatus(const QString &msg);

    void updatePosPixel(const QPointF &pos);

    void updateTreeProperty();
    void updateDrawMat();
    QImage convertMatToImage(const cv::Mat &mat);

private slots:
    void fileNew();
    void fileOpen();
    void fileClose();
    bool fileSave();
    bool fileSaveAs();
    bool fileLoad(const QString &fileName);
    void fileExport();
    void fileExportView();
    void openRecentFile();

    void viewZoomIn();
    void viewZoomOut();
    void viewPreview();
    void viewNext();
    void viewPseudo();
    void viewPseudoConfig();
    void viewGray();
    void viewLengthMeasure();

    void editFilter();
    void editTransform();
    void editRotate();
    void editThreshold();
    void editDilate();
    void editThin();
    void editEdgeCanny();
    void editEdgeSobel();
    void editEdgeRobert();
    void editEdgePrewitt();
    void editEdgeLaplaccian();
    void editEdgeContour();
    void editHistEqualization();

    void roiOpen();
    void roiSave();
    void roiByBox();
    void roiByPolygon();

    void configSystemParam();

    void paintCalibrationNewProject();
    void paintCalibrationViewProject();
    void paintCalibrationSaveProject();
    void paintCalibrationOpenProject();
    void paintCalibrationDataCollection();
    void savePaintCalibrationRange();
    void savePaintCalibrationParameter();
    void paintCalibration();
    void paintCalibrationSave(const QString &msg);
    void paintCalibraionRangeFromGraphicManager();

    void readMarkerFile();
    void saveMarkerFile();
    void addMarkerManual();
    void deleteMarkerManual();
    void pinMarker();
    void autoMarker();
    void referenceMarker();
    void hideMarker();
    void autoSort();
    void fillupMarkers();
    void translateMarker();

    void imageAlignment();
    void sectionalView();
    void sectionalViewProcess(QPointF start, QPointF end);
    void histogram();
    void frequencyDomainView();

    void imageAddition();
    void imageSbstraction();
    void imageDivision();
    void imageAverage();

    void setImgAsBackgound();
    void setImgAsReference();
    void setImgAsPsp();

    void testSubtract();
    void testAdd();
    void testDivide();
    void testAverage();

    void testPyrUp();
    void testPyrDown();

    void testMarkerDetector();
    void testAgastDetector();
    void testAkazeDetector();
    void testBriskDetector();
    void testGfttDetector();
    void testKazeDetetor();
    void testMserDetetor();
    void testOrbDetector();
    void testSimpleBlobDetector();
    void testFilterEnhanceDetector();
    void testCornerSubPix();
    void testFitEllipse();

    void clearTreeProperty();

    void testSurMatch();

    void displayBackgroundImg();
    void displayReferenceImg();
    void displayPspImg();

    void testView3d();

    void testPythonMatPlotLib();
    void testPythonSendMail();

    void testThreshold();
    void testHistGlobalThreshold();
    void testOstuGlobalThreshold();
    void testEntropyGlobalThresold();
    void testTriangleGlobalThreshold();
    void testAdaptiveThreshold();
    void testBilinearInterpolation();
    void testBicubicInterpolation();
    void testNearestNeighborInterpolation();
    void testLANCZOS4();
    void testImage16bytes();
    void testTemplateMatching();
    void testMedianFilter();
    void testGaussianFilter();
    void testBilateralFilter();
    void testSobelX();
    void testSobel();
    void testSobelY();
    void testHistEqualization();
    void testCannyEdge();
    void testContourDetection();
    void testContourFiltering();
    void testContourFit();
    void testSmoothingByFilter();
    void testSmoothingByPyramid();
    void testBrightnessContrastAdjust();

    void testFFT();
    void testIFFT();
    void testBackgroundSubtraction();
    void testHistogram();
    void testErosion();
    void testDilation();
    void MorphologyOpening();
    void MorphologyClosing();
    void MorphologyMorphologicalGradient();
    void MorphologyTop_Hat();
    void MorphologyBlack_Hat();
    void testImageThinning();
    void testSegmentation();
    void testLaplaceSharp();

    void testHoughCircles();
    void testColormap();
    void testColorBar();

    void testAlignment();

    void testPaintCalibrationDataFromDisk();

    Img_io ReadImage(QString title = "");

    void aboutSoftware();
    void openWebsite();

    void manageProject();
    void newProjectSinglComponent();
    void newProjectDualComponent();
    void projectFeatures();
    void openProject();
    void saveProject();
    void saveProjectResults();
    void closeProject();
    void processProject();
    void viewProject();
    void lightRatio();
    void pressureView();
    void pressureCoefficientView();
    void projectCurrentImageWindOn();
    void projectCurrentImageWindOff();
    void projectCurrentImageWindOn2();
    void projectCurrentImageWindOff2();
    void projectCurrentImageBackground();
    void projectCurrentImageBackground2();
    void projectCurrentImageBackground3();
    void projectCurrentImageBackground4();

    void updateMarkerPoints();
    void updateROI();

    void chandLanguageChinese();
    void chandLanguageEnglish();
    void helpManual();

    void emInfo();
    void emitQuit();

private:
    Ui::MainWindow *ui;

    QAction *actionExit;

    QStringList mRecentFiles;
    QString mCurFile;
    enum {
        MaxRecentFileCount = 15
    };
    QAction *mRecentFilesAction[MaxRecentFileCount];

    ImgManager *mImgManager;
    double mScaled;

    bool mIsDocumentModified;

    ViewDisplayWidget *mViewWidget;
    ViewDisp *mViewDisp;

    QVBoxLayout *mMainLayout;
    QHBoxLayout *mHBoxLayout1;
    QHBoxLayout *mHBoxLayout2;

    QTreeWidget *mTreeProperty;
    QListWidget *mListLog;

    QLabel *mLabelMousePos;

    QTranslator *mTranslator;

    PspOperatorUtil *mOperatorUitl;

    PspDocument *mCurrentDocument;
};
