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
#include <QPainter>
#include <QSettings>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QProcess>
#include <string>
#include <QMessageBox>
#include <QCoreApplication>
#include "algo/algo_basic_arithmatic.h"
#include "algo/algo_filter.h"
#include "algo/algo_marker_detector.h"
#include "algo/algo_enhancement.h"
#include "algo/algo_type.h"
#include "base/base_markerpoint.h"
#include "base/base_log.h"
#include "base/base_trace.h"
#include "base/base_system_param.h"
#include "base/base_markerpoint_sort.h"
#include "base/base_graphic_colorbar.h"
#include "base/base_graphic_rect.h"
#include "op/op_manager.h"
#include "op/op_type.h"
#include "op/op_parameter.h"
#include "op/op_markeradd.h"
#include "op/op_factory.h"
#include "op/op_roi_add.h"
#include "manager/manager_doc_two_component.h"
#include "manager/manager_doc_single_component.h"
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
#include <opencv2/ximgproc.hpp>

using namespace PspNsAlgo;
using namespace PspNsBase;
using namespace PspNsOp;

void MainWindow::testSubtract() {
    cv::Mat subtractImg;
    PspNsAlgo::subtract(mImgManager->getReferenceImg(), 0.5 * mImgManager->getBackgroundImg(), subtractImg);

    updateCurMat(subtractImg);

    updateStatus(QString(tr("Image minus test.")));
}

void MainWindow::testAdd() {
    cv::Mat addImg;
    PspNsAlgo::add(mImgManager->getReferenceImg(), mImgManager->getBackgroundImg(), addImg);

    updateCurMat(addImg);

    updateStatus(QString(tr("Image plus test.")));
}

void MainWindow::testDivide() {
    cv::Mat divideImg;
    PspNsAlgo::divide(mImgManager->getReferenceImg(), mImgManager->getBackgroundImg(), divideImg);

    updateCurMat(divideImg);

    updateStatus(QString(tr("Image divide test.")));
}

void MainWindow::testAverage() {
    QStringList fileName = QFileDialog::getOpenFileNames(
            this,
            "Select one or more files to average",
            "/AverageImages",
            "Images (*.tif *.bmp *.jpg)");

    cv::Mat Image1 = cv::imread(fileName.at(0).toStdString(),
                                cv::IMREAD_ANYDEPTH);
    Image1 = convertTo16UC1(Image1);

    qDebug() << "fileLength" << fileName.length();
    cv::Mat average = cv::Mat::zeros(Image1.size(), CV_32FC1);

    int n = fileName.length();
    for (int i = 0; i < n; i++) {
        cv::Mat SrcImg = cv::imread(fileName.at(i).toStdString(), cv::IMREAD_ANYDEPTH);
        SrcImg = convertTo16UC1(SrcImg);
        SrcImg.convertTo(SrcImg, CV_32FC1);

        if (i < n - 1) {
            cv::Mat mat2 = cv::imread(fileName.at(i + 1).toStdString(), cv::IMREAD_ANYDEPTH);
            if (SrcImg.size() != mat2.size()) {
                QErrorMessage *dialog = new QErrorMessage(this);
                dialog->setWindowTitle(tr("Error Messages"));
                dialog->showMessage(tr("The sizes of source images are different!"));
            }
        }

        cv::accumulate(SrcImg, average);
    }
    average = average / n;

    updateCurMat(average);
    updateStatus(QString(tr("Display average image.")));
}

void MainWindow::testPyrUp() {

    cv::Mat src, PryUpImg;
    src = WorkImgManager::GetInstance()->getCurImg();
    cv::pyrUp(src, PryUpImg);
    updateCurMat(PryUpImg);
    updateTreeProperty();
    updateStatus(QString(tr("Dispaly PyrUp image.")));
    update();
}

void MainWindow::testPyrDown() {

    cv::Mat src, PyrDownImg;
    src = WorkImgManager::GetInstance()->getCurImg();
    cv::pyrDown(src, PyrDownImg);
    updateCurMat(PyrDownImg);
    updateTreeProperty();
    updateStatus(QString(tr("Display the PyrDown image")));
}

void MainWindow::testMarkerDetector() {
    vector<QPointF> points;
    cv::Mat markerDetectorImg;

    PspNsAlgo::fastDetector(convertTo8UC1(WorkImgManager::GetInstance()->getCurImg()), points, 30, markerDetectorImg);

    setDrawMatAndMarkerPoints(markerDetectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testFitEllipse() {
    vector<cv::RotatedRect> minEllipse;
    vector<QPointF> points;

    cv::Mat gray, ThreshGray;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    double CannyAccThresh = cv::threshold(gray, ThreshGray, 0, 255, cv::THRESH_OTSU);
    qDebug() << "CannyThresh" << CannyAccThresh;

    PspNsAlgo::fitEllipse(convertTo8UC1(WorkImgManager::GetInstance()->getCurImg()), minEllipse, points);
    cv::Mat drawEllipseMat = WorkImgManager::GetInstance()->getCurImg();
    if (SystemParam::GetInstance()->IsDebugMode()) {
        for (auto iEllipse: minEllipse) {
            ellipse(drawEllipseMat, iEllipse, cv::Scalar(255, 0, 0));
        }
    }

    WorkImgManager::GetInstance()->setCurImg(drawEllipseMat);
    setDrawMatAndMarkerPoints(drawEllipseMat, points);
}

void MainWindow::testAgastDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(convertTo8UC1(WorkImgManager::GetInstance()->getCurImg()), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::agastDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testAkazeDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::akazeDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testBriskDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::briskDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testGfttDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::gfttDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testKazeDetetor() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::kazeDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testMserDetetor() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::mserDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testOrbDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::orbDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image orb detector test.")));
}

void MainWindow::testSimpleBlobDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));

    vector<cv::Mat> channels;
    cv::split(filterImg, channels);

    double lowThreshold, hightThreshold;
    calcOstuThreshold(channels[0], lowThreshold, hightThreshold);

    PspNsAlgo::simpleBlobDetector(filterImg, points, lowThreshold, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testCornerSubPix() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::cornerSubPixDetect(enhanceImg, detectorImg, points);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image corner sub pix detector test.")));
}

void MainWindow::testSurMatch() {
    cv::Mat detectorImg;
    PspNsAlgo::SurfMatch(mImgManager->getBackgroundImg(), mImgManager->getReferenceImg(), detectorImg);

    updateCurMat(detectorImg);

    updateStatus(QString(tr("Image surf match test.")));
}

void MainWindow::testFilterEnhanceDetector() {
    vector<QPointF> points;

    cv::Mat filterImg, enhanceImg, detectorImg;

    PspNsAlgo::filterMean(WorkImgManager::GetInstance()->getCurImg(), filterImg, cv::Size(3, 3));
    PspNsAlgo::filter2D(filterImg, enhanceImg);

    PspNsAlgo::fastDetector(enhanceImg, points, 30, detectorImg);

    setDrawMatAndMarkerPoints(detectorImg, points);

    updateStatus(QString(tr("Image fast detector test.")));
}

void MainWindow::testView3d() {
}

void MainWindow::testPythonMatPlotLib() {

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();

    QString pyPath = dir.absolutePath() + "/python36/python.exe";

    QStringList args;
    args.append("tsT3d.py");

    QProcess::execute(pyPath, args);
}

void MainWindow::testPythonSendMail() {


    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();

    QString pyPath = dir.absolutePath() + "/python36/python.exe";
    QString scritPath = dir.absolutePath() + "/tool/mail_server/sendmail.py";

    QStringList args;
    args.append(scritPath);

    QProcess::execute(pyPath, args);
}

void MainWindow::testBilinearInterpolation() {


    cv::Mat resizeMat;
    PspNsAlgo::resize(WorkImgManager::GetInstance()->getCurImg(), resizeMat, cv::Size(), 2, 2, cv::INTER_LINEAR);

    updateCurMat(resizeMat);
    updateTreeProperty();
    updateStatus(QString(tr("display BilinearInterpolation image.")));
}

void MainWindow::testBicubicInterpolation() {

    cv::Mat resizeMat;
    PspNsAlgo::resize(WorkImgManager::GetInstance()->getCurImg(), resizeMat, cv::Size(), 2, 2, cv::INTER_CUBIC);
    updateTreeProperty();
    updateCurMat(resizeMat);
    updateStatus(QString(tr("display BicubicInterpolation image.")));
}

void MainWindow::testNearestNeighborInterpolation() {

    cv::Mat resizeMat;
    PspNsAlgo::resize(WorkImgManager::GetInstance()->getCurImg(), resizeMat, cv::Size(), 2, 2, cv::INTER_NEAREST);
    updateTreeProperty();
    updateCurMat(resizeMat);
    updateStatus(QString(tr("display BicubicInterpolation image.")));
}

void MainWindow::testLANCZOS4() {

    cv::Mat resizeMat;
    PspNsAlgo::resize(WorkImgManager::GetInstance()->getCurImg(), resizeMat, cv::Size(), 2, 2, cv::INTER_LANCZOS4);
    updateTreeProperty();
    updateCurMat(resizeMat);
    updateStatus(QString(tr("display LANCZOS4Interpolation image.")));
}

void MainWindow::testHoughCircles() {
    cv::Mat src, grayMat;

    grayMat = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    PspNsAlgo::HoughCircles(grayMat, grayMat, cv::HOUGH_GRADIENT, 1, grayMat.rows / 100, 74, 10, 3, 7);
    cvtColor(grayMat, grayMat, cv::COLOR_GRAY2RGB);
    updateCurMat(grayMat);
    updateTreeProperty();
    updateStatus(QString(tr("display HoughCircle image.")));
}

void MainWindow::testColormap() {
    cv::Mat src, dst;
    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    PspNsAlgo::applyColorMap(src, dst, 2);

    const cv::Mat &mat = dst;
    if (mat.empty()) return;
    QImage img((unsigned char *) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    mViewDisp->setDrawImage(img);
    mViewDisp->UpdateView();
    update();
    updateTreeProperty();

    updateStatus(QString(tr("display colormap applied image.")));
}

void MainWindow::testFFT() {
    cv::Mat src, dst, I;
    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
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
    cv::cvtColor(magI, magI, cv::COLOR_GRAY2RGB);
    updateCurMat(magI);
    updateStatus(QString(tr("display Spectrum manitude image.")));
}

void MainWindow::testIFFT() {
    cv::Mat src, dst, img;

    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    src.copyTo(img);

    int M = cv::getOptimalDFTSize(img.rows);
    int N = cv::getOptimalDFTSize(img.cols);
    cv::Mat padded;
    copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[2] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat ImgDFT;
    merge(planes, 2, ImgDFT);

    dft(ImgDFT, ImgDFT);
    ImgDFT = ImgDFT(cv::Rect(0, 0, ImgDFT.cols & -2, ImgDFT.rows & -2));

    int cx = ImgDFT.cols / 2;
    int cy = ImgDFT.rows / 2;
    cv::Mat tmp;
    cv::Mat q0(ImgDFT, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(ImgDFT, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(ImgDFT, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(ImgDFT, cv::Rect(cx, cy, cx, cy));
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    split(ImgDFT, planes);
    cv::Mat Proc_RE = planes[0];
    cv::Mat Proc_IM = planes[1];

#if 1
    cv::Mat mag;
    magnitude(planes[0], planes[1], mag);
    mag += cv::Scalar::all(1);
    log(mag, mag);
    normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    imshow("spectrum magnitude", mag);

#endif
    merge(planes, 2, ImgDFT);
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    idft(ImgDFT, ImgDFT, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

    cv::Mat imp = ImgDFT;
    normalize(imp, imp, 0, 255, cv::NORM_MINMAX);
    imp.convertTo(imp, CV_8U);
    cv::Mat Output = imp(cv::Range(0, img.rows), cv::Range(0, img.cols));
    imshow("Output", Output);
}

void MainWindow::testBackgroundSubtraction() {
    cv::Mat src, dst, img;
    QString fileName = QFileDialog::getOpenFileName(this, "Open Background", QDir::currentPath(),
                                                    "Images(*.tif *.jpg *.png *.bmp)");
    cv::Mat BackgroundImg = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    src = BackgroundImg;

    QString fileName2 = QFileDialog::getOpenFileName(this, "Open test image", QDir::currentPath(),
                                                     "Images(*.tif *.jpg *.png *.bmp)");
    cv::Mat TestImg = cv::imread(fileName2.toStdString(), cv::IMREAD_COLOR);


    cvtColor(src, src, cv::COLOR_RGB2GRAY);
    cvtColor(TestImg, TestImg, cv::COLOR_RGB2GRAY);

    src.copyTo(img);

    int M = cv::getOptimalDFTSize(img.rows);
    int N = cv::getOptimalDFTSize(img.cols);
    cv::Mat padded;
    copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[2] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat ImgDFT;
    merge(planes, 2, ImgDFT);

    dft(ImgDFT, ImgDFT);

    ImgDFT = ImgDFT(cv::Rect(0, 0, ImgDFT.cols & -2, ImgDFT.rows & -2));

    int cx = ImgDFT.cols / 2;
    int cy = ImgDFT.rows / 2;
    cv::Mat tmp;
    cv::Mat q0(ImgDFT, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(ImgDFT, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(ImgDFT, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(ImgDFT, cv::Rect(cx, cy, cx, cy));
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    split(ImgDFT, planes);
    cv::Mat Proc_RE = planes[0];
    cv::Mat Proc_IM = planes[1];

    cv::Mat img2;
    TestImg.copyTo(img2);

    int M2 = cv::getOptimalDFTSize(img2.rows);
    int N2 = cv::getOptimalDFTSize(img2.cols);
    cv::Mat padded2;
    copyMakeBorder(img2, padded2, 0, M2 - img2.rows, 0, N2 - img2.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes2[2] = {cv::Mat_<float>(padded2), cv::Mat::zeros(padded2.size(), CV_32F)};
    cv::Mat ImgDFT2;
    merge(planes2, 2, ImgDFT2);

    dft(ImgDFT2, ImgDFT2);
    ImgDFT2 = ImgDFT2(cv::Rect(0, 0, ImgDFT2.cols & -2, ImgDFT2.rows & -2));

    int cx2 = ImgDFT2.cols / 2;
    int cy2 = ImgDFT2.rows / 2;
    cv::Mat tmp2;
    cv::Mat q0_2(ImgDFT2, cv::Rect(0, 0, cx2, cy2));
    cv::Mat q1_2(ImgDFT2, cv::Rect(cx2, 0, cx2, cy2));
    cv::Mat q2_2(ImgDFT2, cv::Rect(0, cy2, cx2, cy2));
    cv::Mat q3_2(ImgDFT2, cv::Rect(cx2, cy2, cx2, cy2));
    q0_2.copyTo(tmp2);
    q3_2.copyTo(q0_2);
    tmp2.copyTo(q3_2);

    q1_2.copyTo(tmp2);
    q2_2.copyTo(q1_2);
    tmp2.copyTo(q2_2);
    split(ImgDFT2, planes2);
    cv::Mat Proc_RE2 = planes2[0];
    cv::Mat Proc_IM2 = planes2[1];

    for (int i = 0; i < Proc_RE.rows; ++i)
        for (int j = 0; j < Proc_RE.cols; ++j) {
            Proc_RE.at<float>(i, j) = Proc_RE2.at<float>(i, j) - Proc_RE.at<float>(i, j);
            Proc_IM.at<float>(i, j) = Proc_IM2.at<float>(i, j) - Proc_IM.at<float>(i, j);
        }
#if 1
    cv::Mat mag;
    magnitude(planes[0], planes[1], mag);
    mag += cv::Scalar::all(1);
    log(mag, mag);
    normalize(mag, mag, 0, 1, cv::NORM_MINMAX);

#endif
    merge(planes, 2, ImgDFT);
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    idft(ImgDFT, ImgDFT, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
    cv::Mat imp = ImgDFT;
    normalize(imp, imp, 0, 255, cv::NORM_MINMAX);
    imp.convertTo(imp, CV_8U);
    cv::Mat Output = imp(cv::Range(0, img.rows), cv::Range(0, img.cols));
    imshow("BackgroundImg", BackgroundImg);
    imshow("TestImg", TestImg);
    cvtColor(Output, Output, cv::COLOR_GRAY2RGB);
    updateTreeProperty();
    updateCurMat(Output);
}

void MainWindow::testHistogram() {
    cv::Mat src, dst, srcImage;
    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    src.copyTo(srcImage);

    int channels = 0;
    cv::MatND dstHist;
    int histSize[] = {256};
    float midRanges[] = {0, 256};
    const float *ranges[] = {midRanges};
    calcHist(&srcImage, 1, &channels, cv::Mat(), dstHist, 1, histSize, ranges, true, false);

    cv::Mat drawImage = cv::Mat::zeros(cv::Size(256, 256), CV_8UC3);
    double g_dHistMaxValue;
    minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
    for (int i = 0; i < 256; i++) {
        int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);
        line(drawImage, cv::Point(i, drawImage.rows - 1), cv::Point(i, drawImage.rows - 1 - value),
             cv::Scalar(0, 0, 255));
    }

    updateTreeProperty();
    updateCurMat(drawImage);
    updateStatus(QString(tr("display Histogram image.")));
}

void MainWindow::testErosion() {
    cv::Mat src, erosion_dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int erosion_elem = 1;
    int erosion_size = 2;

    int erosion_type = 0;
    if (erosion_elem == 0) { erosion_type = cv::MORPH_RECT; }
    else if (erosion_elem == 1) { erosion_type = cv::MORPH_CROSS; }
    else if (erosion_elem == 2) { erosion_type = cv::MORPH_ELLIPSE; }

    cv::Mat element = getStructuringElement(erosion_type,
                                            cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                            cv::Point(erosion_size, erosion_size));

    erode(src, erosion_dst, element);

    updateTreeProperty();
    updateCurMat(erosion_dst);
    updateStatus(QString(tr("display Erosion image.")));
}

void MainWindow::testDilation() {
    cv::Mat src, dilation_dst;
    src = WorkImgManager::GetInstance()->getCurImg();

    int dilation_elem = 1;
    int dilation_size = 2;
    int const max_elem = 2;
    int const max_kernel_size = 21;

    int dilation_type = 0;
    if (dilation_elem == 0) { dilation_type = cv::MORPH_RECT; }
    else if (dilation_elem == 1) { dilation_type = cv::MORPH_CROSS; }
    else if (dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }
    cv::Mat element = getStructuringElement(dilation_type,
                                            cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                            cv::Point(dilation_size, dilation_size));

    dilate(src, dilation_dst, element);

    updateTreeProperty();
    updateCurMat(dilation_dst);
    updateStatus(QString(tr("display dilation image.")));
}

void MainWindow::testImageThinning() {
    cv::Mat src, dst;
    src = WorkImgManager::GetInstance()->getCurImg();
    src = convertTo8UC1(src);
    cv::ximgproc::thinning(src, dst, 0);
    WorkImgManager::GetInstance()->setCurImg(dst);
}

void MainWindow::testLaplaceSharp() {
    cv::Mat src, dstImg;
    src = WorkImgManager::GetInstance()->getCurImg();

    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
    cv::Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    cv::Mat sharp;
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    updateTreeProperty();
    updateCurMat(imgResult);
    updateStatus(QString(tr("display LaplaceSharp image.")));
}

void MainWindow::testSegmentation() {
    cv::Mat src, dstImg;
    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    cvtColor(src, src, cv::COLOR_GRAY2RGB);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            if (src.at<cv::Vec3b>(i, j) == cv::Vec3b(255, 255, 255)) {
                src.at<cv::Vec3b>(i, j)[0] = 0;
                src.at<cv::Vec3b>(i, j)[1] = 0;
                src.at<cv::Vec3b>(i, j)[2] = 0;
            }
        }
    }
    imshow("Black Background Image", src);

    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
    cv::Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    cv::Mat sharp;
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;

    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);

    imshow("New Sharped Image", imgResult);

    cv::Mat bw;
    cvtColor(imgResult, bw, cv::COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    imshow("Binary Image", bw);

    cv::Mat dist;
    distanceTransform(bw, dist, cv::DIST_L2, 3);

    normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);
    imshow("Distance Transform Image", dist);

    threshold(dist, dist, 0.4, 1.0, cv::THRESH_BINARY);

    cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, kernel1);
    imshow("Peaks", dist);

    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);

    vector<vector<cv::Point> > contours;
    findContours(dist_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32S);

    for (size_t i = 0; i < contours.size(); i++) {
        drawContours(markers, contours, static_cast<int>(i), cv::Scalar(static_cast<int>(i) + 1), -1);
    }

    watershed(imgResult, markers);
    cv::Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);

    vector<cv::Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++) {
        int b = cv::theRNG().uniform(0, 256);
        int g = cv::theRNG().uniform(0, 256);
        int r = cv::theRNG().uniform(0, 256);
        colors.push_back(cv::Vec3b((uchar) b, (uchar) g, (uchar) r));
    }

    cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);
    for (int i = 0; i < markers.rows; i++) {
        for (int j = 0; j < markers.cols; j++) {
            int index = markers.at<int>(i, j);
            if (index > 0 && index <= static_cast<int>(contours.size())) {
                dst.at<cv::Vec3b>(i, j) = colors[index - 1];
            }
        }
    }
    imshow("Final Result", dst);
}

void MainWindow::testCannyEdge() {
    cv::Mat gray, ThreshGray;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    double fscale = 1;
    cv::resize(gray, gray, cv::Size(), fscale, fscale, cv::INTER_CUBIC);

    double lowThreshold, highThreshold;
    calcOstuThreshold(gray, lowThreshold, highThreshold);

    Canny(gray, gray, lowThreshold * 0.5, highThreshold * 0.5);

    cvtColor(gray, gray, cv::COLOR_GRAY2RGB);
    updateCurMat(gray);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display CannyEdge image.")));
}

void MainWindow::testContourDetection() {

    cv::Mat gray, ThreshGray;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    double fscale = 1;
    cv::resize(gray, gray, cv::Size(), fscale, fscale, cv::INTER_CUBIC);
    double lowThreshold, highThreshold;
    calcOstuThreshold(gray, lowThreshold, highThreshold);

    Canny(gray, gray, lowThreshold * 0.5, highThreshold * 0.5);

    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    findContours(gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    double largest_size = 0;
    int largest_contour_index;
    for (int i = 0; i < contours.size(); i++) {
        double a = contours[i].size();
        if (a > largest_size) {
            largest_size = a;
            largest_contour_index = i;
        }
    }

    cv::Mat clone;
    cv::resize(WorkImgManager::GetInstance()->getCurImg(), clone, cv::Size(), fscale, fscale, cv::INTER_CUBIC);
    cv::Mat dst = cv::Mat::zeros(clone.rows, clone.cols, CV_8UC3);

    int idx = 0;
    for (; idx >= 0; idx = hierarchy[idx][0]) {
        cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
        drawContours(dst, contours, idx, color, cv::FILLED, 8, hierarchy);
    }

    updateCurMat(dst);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display Contour image.")));
}

void MainWindow::testContourFiltering() {
    cv::Mat gray, ThreshGray;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    double fscale = 1;
    cv::resize(gray, gray, cv::Size(), fscale, fscale, cv::INTER_CUBIC);

    double lowThreshold, highThreshold;
    calcOstuThreshold(gray, lowThreshold, highThreshold);

    Canny(gray, gray, lowThreshold * 0.5, highThreshold * 0.5);

    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    findContours(gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    double largest_size = 0;
    int largest_contour_index;
    for (int i = 0; i < contours.size(); i++) {
        double a = contours[i].size();
        if (a > largest_size) {
            largest_size = a;
            largest_contour_index = i;
        }
    }

    double SencondLargestSize = 0;
    int SecondLargestContuorIndex;
    for (int i = 0; i < contours.size(); i++) {
        double a = contours[i].size();
        if (a > SencondLargestSize && a < largest_size) {
            SencondLargestSize = a;
            SecondLargestContuorIndex = i;
        }
    }

    cv::Mat clone;
    cv::resize(WorkImgManager::GetInstance()->getCurImg(), clone, cv::Size(), fscale, fscale, cv::INTER_CUBIC);
    cv::Mat dst = cv::Mat::zeros(clone.rows, clone.cols, CV_8UC3);

    int idx = 0;
    for (; idx >= 0; idx = hierarchy[idx][0]) {
        if (contours[idx].size() > largest_size - 10) {
            cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
            drawContours(dst, contours, idx, color, cv::FILLED, 8, hierarchy);
            drawContours(clone, contours, idx, color, cv::FILLED, 8, hierarchy);
        }
    }

    updateCurMat(dst);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display Contour image.")));
}

void MainWindow::testSmoothingByFilter() {

    QString fileName = QFileDialog::getOpenFileName(this, "Open a binary image", QDir::currentPath(),
                                                    "Images(*.tif *.jpg *.png *.bmp)");

    cv::Mat im = cv::imread(fileName.toStdString(), 0);
    cv::Mat cont = ~im;
    cv::Mat original = cv::Mat::zeros(im.rows, im.cols, CV_8UC3);
    cv::Mat smoothed = cv::Mat(im.rows, im.cols, CV_8UC3, cv::Scalar(255, 255, 255));

    int filterRadius = 5;
    int filterSize = 2 * filterRadius + 1;
    double sigma = 10;
    int idx = 0;

    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;

    findContours(cont, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    for (size_t j = 0; j < contours.size(); j++) {
        size_t len = contours[j].size() + 2 * filterRadius;
        size_t idx = (contours[j].size() - filterRadius);
        vector<float> x, y;
        for (size_t i = 0; i < len; i++) {
            x.push_back(contours[j][(idx + i) % contours[j].size()].x);
            y.push_back(contours[j][(idx + i) % contours[j].size()].y);
        }
        vector<float> xFilt, yFilt;
        GaussianBlur(x, xFilt, cv::Size(filterSize, filterSize), sigma, sigma);
        GaussianBlur(y, yFilt, cv::Size(filterSize, filterSize), sigma, sigma);
        vector<vector<cv::Point> > smoothContours;
        vector<cv::Point> smooth;
        for (size_t i = filterRadius; i < contours[j].size() + filterRadius; i++) {
            smooth.push_back(cv::Point(xFilt[i], yFilt[i]));
        }
        smoothContours.push_back(smooth);

        cv::Scalar color;

        if (hierarchy[j][3] < 0) {
            color = cv::Scalar(0, 0, 0);
        } else {
            color = cv::Scalar(255, 255, 255);
        }

        drawContours(smoothed, smoothContours, -1, color, cv::FILLED, 4);
    }

    updateCurMat(smoothed);
    updateTreeProperty();
    updateStatus(QString(tr("display Smoothed Contour image.")));
}

void MainWindow::testSmoothingByPyramid() {
    cv::Mat vesselImage = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    imshow("original image", vesselImage);
    cv::threshold(vesselImage, vesselImage, 125, 255, cv::THRESH_BINARY);
    cv::Mat blurredImage;
    cv::pyrUp(vesselImage, blurredImage);

    for (int i = 0; i < 15; i++)
        cv::medianBlur(blurredImage, blurredImage, 7);

    cv::pyrDown(blurredImage, blurredImage);
    cv::threshold(blurredImage, blurredImage, 200, 255, cv::THRESH_BINARY);
    updateCurMat(blurredImage);
    updateTreeProperty();
    updateStatus(QString(tr("Display contour smoothed image")));

}

void MainWindow::testContourFit() {
    double fscale = 1;
    QString strs;
    cv::Mat dst;
    QStringList file_list, output_name;
    QStringList str_path_list = QFileDialog::getOpenFileNames(this, tr("Open images"), tr("/"),
                                                              tr("Images(*.tif *.jpg)"));
    for (int i = 0; i < str_path_list.size(); i++) {
        QString str_path = str_path_list[i];
        QString fileName, Img_name, file_path, file_suffix, ImgSaveName, fileBaseName;
        QFileInfo fileinfo;
        fileName = str_path;
        fileinfo = QFileInfo(fileName);

        file_path = fileinfo.absolutePath();
        fileBaseName = fileinfo.baseName();
        file_suffix = fileinfo.suffix();

        ImgSaveName = file_path + "/" + "shock_" + fileBaseName + "." + file_suffix;

        cv::Mat src = cv::imread(fileName.toStdString(), 0);
        cv::Mat gray = convertTo8UC1(src);

        cv::resize(gray, gray, cv::Size(), fscale, fscale, cv::INTER_LANCZOS4);

        int FilterSize = 3 + (fscale - 1) * 2;

        int isMedianBlur = 1;
        int isGaussianBlur = 1;
        int isBilateralBlur = 0;

        if (isMedianBlur) { cv::medianBlur(gray, gray, 3); }
        if (isGaussianBlur) { cv::GaussianBlur(gray, gray, cv::Size(FilterSize, FilterSize), 0.0, 0.0); }
        if (isBilateralBlur) {
            cv::Mat gray2;
            gray2 = convertTo8UC1(gray);
            PspNsAlgo::filterBilateral(gray2, gray, -1, 6, 6, cv::BORDER_DEFAULT);
        }

        int isAdaptiveThreshold = 1;
        if (isAdaptiveThreshold) {
            const int maxVal = 255;
            int IsThickFeature = 0;
            if (IsThickFeature) {
                int blockSize = 15;
                int constValue = 8;
                int adaptiveMethod = 0;
                int thresholdType = 1;
                PspNsAlgo::adaptiveThreshold(gray, gray, maxVal, adaptiveMethod, thresholdType, blockSize, constValue);
            } else {
                int blockSize = 25;
                int constValue = 5;
                int adaptiveMethod = 1;
                int thresholdType = 1;
                PspNsAlgo::adaptiveThreshold(gray, gray, maxVal, adaptiveMethod, thresholdType, blockSize, constValue);
            }
        }

        double lowThreshold, highThreshold;
        calcOstuThreshold(gray, lowThreshold, highThreshold);

        Canny(gray, gray, lowThreshold * 0.5, highThreshold * 0.5);

        vector<vector<cv::Point> > contours;
        vector<cv::Vec4i> hierarchy;
        findContours(gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        double largest_size = 0;
        int largest_contour_index;
        for (int i = 0; i < contours.size(); i++) {
            double a = contours[i].size();
            if (a > largest_size) {
                largest_size = a;
                largest_contour_index = i;
            }
        }

        double SencondLargestSize = 0;
        int SecondLargestContuorIndex;
        for (int i = 0; i < contours.size(); i++) {
            double a = contours[i].size();
            if (a > SencondLargestSize && a < largest_size) {
                SencondLargestSize = a;
                SecondLargestContuorIndex = i;
            }
        }

        int isContourFilter = 1;
        int isDrawContourOnSrcImg = 0;
        cv::Mat clone;
        cv::resize(src, clone, cv::Size(), fscale, fscale, cv::INTER_CUBIC);
        cv::Mat dst = cv::Mat::zeros(clone.rows, clone.cols, CV_8UC3);

        if (isContourFilter) {
            int idx = 0;
            for (; idx >= 0; idx = hierarchy[idx][0]) {
                if (contours[idx].size() > SencondLargestSize - 100) {
                    cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
                    if (isDrawContourOnSrcImg) {
                        drawContours(clone, contours, idx, cv::Scalar(0, 0, 0), cv::FILLED, 8, hierarchy);
                    } else { drawContours(dst, contours, idx, color, cv::FILLED, 8, hierarchy); }
                }
            }
        } else {
            int idx = 0;
            for (; idx >= 0; idx = hierarchy[idx][0]) {
                cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
                if (!isDrawContourOnSrcImg) { drawContours(dst, contours, idx, color, cv::FILLED, 8, hierarchy); }
                else { drawContours(clone, contours, idx, cv::Scalar(0, 0, 0), cv::FILLED, 8, hierarchy); }
            }
        }

        int isConvertGray = 1;
        int isInvertBinary = 1;
        if (isDrawContourOnSrcImg) {
            if (isConvertGray) { clone = convertTo8UC1(clone); }
            if (isInvertBinary) { PspNsAlgo::BasicThreshold(clone, clone, 10, 255, 1); }
            cv::imwrite(ImgSaveName.toStdString(), clone);
            updateCurMat(clone);
        } else {
            if (isConvertGray) { dst = convertTo8UC1(dst); }
            if (isInvertBinary) { PspNsAlgo::BasicThreshold(dst, dst, 10, 255, 1); }

            cv::imwrite(ImgSaveName.toStdString(), dst);

            updateCurMat(dst);
        }
    }

    update();
    updateTreeProperty();
    updateStatus(QString(tr("display Contour image.")));
}

void MainWindow::testThreshold() {
    cv::Mat dst, src_gray;

    cv::Mat gray, ThreshGray;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    double CannyAccThresh = cv::threshold(gray, ThreshGray, 0, 255, cv::THRESH_TRIANGLE);
    qDebug() << "CannyThresh" << CannyAccThresh;

    double threshold_value = CannyAccThresh * 0.9;
    double maxval = 255;
    int threshold_type = 1;
    qDebug() << "CannyThresh" << threshold_value;

    double mCurrentThreshold;
    double highThreshold;
    calcAdapiveThreshold(gray, mCurrentThreshold, highThreshold);
    qDebug() << "MeanThresh" << mCurrentThreshold;
    threshold_value = mCurrentThreshold;

    cv::Mat thresholdMat;
    PspNsAlgo::BasicThreshold(convertTo8UC1(WorkImgManager::GetInstance()->getCurImg()), thresholdMat, threshold_value,
                              maxval, threshold_type);

    updateCurMat(thresholdMat);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display thresholded image.")));
}

void MainWindow::testHistGlobalThreshold() {
}

void MainWindow::testOstuGlobalThreshold() {
    double OstuThreshValue;
    cv::Mat gray, ThreshGray, thresholdMat;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    OstuThreshValue = cv::threshold(gray, ThreshGray, 0, 255, cv::THRESH_OTSU);
    qDebug() << "OstuThreshValue" << OstuThreshValue;

    PspNsAlgo::BasicThreshold(gray, thresholdMat, 0.8 * OstuThreshValue, 255, 0);

    updateCurMat(thresholdMat);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display Ostu thresholded image.")));
}

void MainWindow::testEntropyGlobalThresold() {

}

void MainWindow::testTriangleGlobalThreshold() {
    double TriangleThreshValue;
    double ScaleFactor = 1;

    cv::Mat gray, ThreshGray, thresholdMat;
    gray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    TriangleThreshValue = cv::threshold(gray, ThreshGray, 0, 255, cv::THRESH_TRIANGLE);


    int IsThreshToBinary = 1;
    if (IsThreshToBinary) {
        qDebug() << "TriangleThreshValue" << TriangleThreshValue;
        PspNsAlgo::BasicThreshold(gray, thresholdMat, ScaleFactor * TriangleThreshValue, 255, 0);
    }

    int IsThreshToZero = 0;
    if (IsThreshToZero) {
        PspNsAlgo::BasicThreshold(gray, thresholdMat, ScaleFactor * TriangleThreshValue, 255, cv::THRESH_TOZERO);
    }

    updateCurMat(thresholdMat);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display Triangle thresholded image.")));
}

void MainWindow::testPaintCalibrationDataFromDisk() {
    if (PspDocPaintCalibration *document = dynamic_cast<PspDocPaintCalibration *>(mCurrentDocument)) {
        PaintCalibrationDataFromDisk::GetInstance()->Init(document);
    }
}

void MainWindow::testBrightnessContrastAdjust() {
    cv::Mat image = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    cv::Mat new_image = cv::Mat::zeros(image.size(), image.type());
    double alpha = 1.3;
    int beta = 0;

    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            for (int c = 0; c < image.channels(); c++) {
                new_image.at<cv::Vec3b>(y, x)[c] =
                        cv::saturate_cast<uchar>(alpha * image.at<cv::Vec3b>(y, x)[c] + beta);
            }
        }
    }

    updateCurMat(new_image);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("adjust the brightness and contrast")));
}

void MainWindow::testAdaptiveThreshold() {
    cv::Mat srcGray;
    srcGray = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    cv::Mat dstImage;

    const int maxVal = 255;

    int IsThickFeature = 0;

    if (IsThickFeature) {
        int blockSize = 15;
        int constValue = 8;
        int adaptiveMethod = 0;
        int thresholdType = 1;
        PspNsAlgo::adaptiveThreshold(srcGray, dstImage, maxVal, adaptiveMethod, thresholdType, blockSize, constValue);
    } else {
        int blockSize = 25;
        int constValue = 5;
        int adaptiveMethod = 1;
        int thresholdType = 1;
        PspNsAlgo::adaptiveThreshold(srcGray, dstImage, maxVal, adaptiveMethod, thresholdType, blockSize, constValue);
    }

    updateCurMat(dstImage);
    update();
    updateTreeProperty();
    updateStatus(QString(tr("display adaptiveThresholded image.")));
}

void MainWindow::testMedianFilter() {


    cv::Mat src, filterMedianMat;
    int FilterSize = 3;
    int Iteration = 1;
    src = WorkImgManager::GetInstance()->getCurImg();
    src.copyTo(filterMedianMat);
    filterMedianMat = convertTo8UC1(src);

    for (int i = 0; i < Iteration; i++) {
        PspNsAlgo::filterMedian(filterMedianMat, filterMedianMat, FilterSize);

    }

    updateCurMat(filterMedianMat);
    update();
    updateStatus(QString(tr("display Median filtered image.")));

}

void MainWindow::testGaussianFilter() {


    cv::Mat src, gaussianMat;

    int FilterSize = 3;
    int Iteration = 1;

    src = WorkImgManager::GetInstance()->getCurImg();
    src.copyTo(gaussianMat);
    for (int i = 0; i < Iteration; i++) {
        PspNsAlgo::filterGaussian(gaussianMat, gaussianMat, cv::Size(FilterSize, FilterSize), 0, 0, cv::BORDER_DEFAULT);
    }
    updateCurMat(gaussianMat);
    update();
    updateStatus(QString(tr("display Gaussian filtered image.")));
}

void MainWindow::testBilateralFilter() {
    cv::Mat src, BilateralMat;
    src = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    int FilterSize = 3;
    int Iteration = 1;
    for (int i = 0; i < Iteration; i++) {
        PspNsAlgo::filterBilateral(src, BilateralMat, FilterSize, FilterSize * 2, FilterSize / 2, cv::BORDER_DEFAULT);
    }

    updateCurMat(BilateralMat);
    update();
    updateStatus(QString(tr("display Bilateral filtered image.")));
}

void MainWindow::testSobelX() {
    cv::Mat image = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    cv::Mat sobelX;
    cv::Sobel(image, sobelX, CV_8U, 1, 0, 3, 0.4, 128);

    updateCurMat(sobelX);
    update();
    updateStatus(QString(tr("display SobelX image.")));
}

void MainWindow::testSobel() {

    cv::Mat image = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    cv::Mat sobelX;
    cv::Sobel(image, sobelX, CV_8U, 1, 0, 3, 0.4, 128);

    cv::Mat sobelY;
    cv::Sobel(image, sobelY, CV_8U, 0, 1, 3, 0.4, 128);

    cv::Sobel(image, sobelX, CV_16S, 1, 0);
    cv::Sobel(image, sobelY, CV_16S, 0, 1);
    cv::Mat sobel;

    sobel = abs(sobelX) + abs(sobelY);

    double sobmin, sobmax;
    cv::minMaxLoc(sobel, &sobmin, &sobmax);

    cv::Mat sobelImage;
    sobel.convertTo(sobelImage, CV_8U, -255. / sobmax, 255);

    updateCurMat(sobelImage);
    update();
    updateStatus(QString(tr("display Sobel image.")));
}

void MainWindow::testSobelY() {
    cv::Mat image = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());

    cv::Mat sobelX;
    cv::Sobel(image, sobelX, CV_8U, 1, 0, 3, 0.4, 128);

    cv::Mat sobelY;
    cv::Sobel(image, sobelY, CV_8U, 0, 1, 3, 0.4, 128);

    updateCurMat(sobelY);
    update();
    updateStatus(QString(tr("display SobelY image.")));
}

void MainWindow::testHistEqualization() {
    cv::Mat dstMat;
    dstMat = convertTo8UC1(WorkImgManager::GetInstance()->getCurImg());
    PspNsAlgo::equalizeHist(dstMat, dstMat);
    updateCurMat(dstMat);
    update();
    updateStatus(QString(tr("display hiseEqualized image.")));
}

void MainWindow::testTemplateMatching() {
    Img_io io1 = ReadImage();
    Img_io io2 = ReadImage();

    bool use_mask;
    cv::Mat img;
    cv::Mat templ;
    cv::Mat mask;
    cv::Mat result;
    cv::Mat TemplateSrc;
    const char *image_window = "Source Image";
    namedWindow(image_window, cv::WINDOW_AUTOSIZE);
    int match_method = cv::TM_CCORR_NORMED;

    img = io2.getSrcImg();
    TemplateSrc = io1.getSrcImg();

    cv::Mat ROI(TemplateSrc, cv::Rect(1534, 903, 20, 20));
    cv::rectangle(TemplateSrc, cv::Point(1534, 903), cv::Point(1534 + 20, 903 + 20), cv::Scalar::all(0), 2, 8, 0);
    imshow("TemplateSrcImg", TemplateSrc);
    templ = ROI;

    use_mask = false;
    cv::Mat img_display;
    img.copyTo(img_display);
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);
    bool method_accepts_mask = (cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED);
    if (use_mask && method_accepts_mask) { matchTemplate(img, templ, result, match_method, mask); }
    else { matchTemplate(img, templ, result, match_method); }
    normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED) { matchLoc = minLoc; }
    else { matchLoc = maxLoc; }
    rectangle(img_display, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar::all(0), 2,
              8, 0);
    rectangle(result, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar::all(0), 2, 8,
              0);

    imshow(image_window, img_display);
    cv::waitKey(0);
}

void MainWindow::testAlignment() {

    QString mat1Path = "";
    QString mat2Path = "";
    Img_io io;
    io.readImg(cv::String(mat1Path.toLocal8Bit().data()));
    cv::Mat mat1 = io.getSrcImg();
    io.readImg(cv::String(mat2Path.toLocal8Bit().data()));
    cv::Mat mat2 = io.getSrcImg();

    const vector<MarkerPoint> &mp1 = readMarkerOutside();
    const vector<MarkerPoint> &mp2 = readMarkerOutside();

    vector<cv::Point2f> kp1;
    vector<cv::Point2f> kp2;
    for (auto point: mp1) {
        kp1.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
    }
    for (auto point: mp2) {
        kp2.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
    }

    cv::Mat matrix;
    PspNsAlgo::FindHomography(kp1, kp2, matrix, cv::RANSAC);

    cv::Mat dst;
    cv::Mat invMatrix = matrix.inv();
    cv::warpPerspective(mat2, dst, invMatrix, cv::Size(mat2.cols, mat2.rows));

    qDebug() << "invMatrix(0,0):" << ((double *) invMatrix.ptr(0))[0];
    qDebug() << "invMatrix(0,1):" << ((double *) invMatrix.ptr(0))[1];
    qDebug() << "invMatrix(0,2):" << ((double *) invMatrix.ptr(0))[2];
    qDebug() << "invMatrix(1,0):" << ((double *) invMatrix.ptr(1))[0];
    qDebug() << "invMatrix(1,1):" << ((double *) invMatrix.ptr(1))[1];
    qDebug() << "invMatrix(1,2):" << ((double *) invMatrix.ptr(1))[2];
    qDebug() << "invMatrix(2,0):" << ((double *) invMatrix.ptr(2))[0];
    qDebug() << "invMatrix(2,1):" << ((double *) invMatrix.ptr(2))[1];
    qDebug() << "invMatrix(2,2):" << ((double *) invMatrix.ptr(2))[2];


    vector<float> xOffset, yOffset;
    for (size_t i = 0; i < kp2.size(); i++) {
        float x = kp2[i].x, y = kp2[i].y;
        float t = x * ((double *) invMatrix.ptr(2))[0] + y * ((double *) invMatrix.ptr(2))[1] +
                  ((double *) invMatrix.ptr(2))[2];
        xOffset.push_back((x * ((double *) invMatrix.ptr(0))[0] + y * ((double *) invMatrix.ptr(0))[1] +
                           ((double *) invMatrix.ptr(0))[2]) / t);
        yOffset.push_back((x * ((double *) invMatrix.ptr(1))[0] + y * ((double *) invMatrix.ptr(1))[1] +
                           ((double *) invMatrix.ptr(1))[2]) / t);
    }

    float xDeviationMax = 0.0, xDeviationMean = 0.0, xDeviationSum = 0.0;
    float yDeviationMax = 0.0, yDeviationMean = 0.0, yDeviationSum = 0.0;
    float disDeviationMax = 0.0, disDeviationMean = 0.0, disDeviationSum = 0.0;
    for (int i = 0; i < kp2.size(); i++) {
        float valX = xOffset[i] - kp1[i].x;
        qDebug() << "Offset X" << i << ":" << kp2[i].x - kp1[i].x;
        qDebug() << "Offset Y" << i << ":" << kp2[i].y - kp1[i].y;
        qDebug() << "valX" << i << ":" << valX;
        xDeviationSum += valX;

        if (fabs(valX) > xDeviationMax) {
            xDeviationMax = fabs(valX);
        }

        float valY = yOffset[i] - kp1[i].y;
        qDebug() << "valY" << i << ":" << valY;

        yDeviationSum += valY;

        if (fabs(valY) > yDeviationMax) {
            yDeviationMax = fabs(valY);
        }

        float valDis = sqrt(valX * valX + valY * valY);
        disDeviationSum += valDis;
        if (valDis > disDeviationMax) {
            disDeviationMax = valDis;
        }
    }
    xDeviationMean = xDeviationSum / (float) kp2.size();
    yDeviationMean = yDeviationSum / (float) kp2.size();
    disDeviationMean = disDeviationSum / (float) kp2.size();
    qDebug() << "x deviation max: " << xDeviationMax << " y deviation max: " << yDeviationMax
             << " distance deviation max: " << disDeviationMax;
    qDebug() << "x deviation mean: " << xDeviationMean << " y deviation mean: " << yDeviationMean
             << " distance deviation mean: " << disDeviationMean;

    cv::Mat DivideImg;
    PspNsAlgo::divide(mat1, dst, DivideImg);
    DivideImg *= 80;
    DivideImg.convertTo(DivideImg, CV_8UC1);
    cv::cvtColor(DivideImg, DivideImg, cv::COLOR_GRAY2RGB);
    WorkImgManager::GetInstance()->setCurImg(DivideImg);

    dst *= 1. / 255;
    dst.convertTo(dst, CV_8UC1);
    cv::cvtColor(dst, dst, cv::COLOR_GRAY2RGB);

    updateCurMat(dst);
}

void MainWindow::testColorBar() {
    GraphicColorBar::GetInstance()->SetColorMin(0);
    GraphicColorBar::GetInstance()->SetColorMax(255);
    SystemParam::GetInstance()->SetIsShowPseudoColor(true);
}

void MainWindow::testImage16bytes() {
    cv::Mat dstMat;
    WorkImgManager::GetInstance()->getCurImg().convertTo(dstMat, CV_16U);

    updateCurMat(dstMat);
    update();
    updateTreeProperty();
}
