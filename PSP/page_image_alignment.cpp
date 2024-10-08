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
#include <QQmlContext>
#include <QFileDialog>
#include <QMessageBox>
#include "page_image_alignment.h"
#include "ui_page_image_alignment.h"
#include "psp_utils_json_process.h"
#include "algo/algo_match.h"
#include "algo/algo_basic_arithmatic.h"
#include "algo/algo_threshold.h"
#include "algo/algo_filter.h"
#include "io/io.h"
#include "manager/work_manager.h"
#include "base/base_log.h"
#include "base/base_graphic_line.h"
#include "base/base_graphic_text.h"
#include "base/base_graphic_colorbar.h"
#include "base/base_graphic_manager.h"
#include "base/base_system_param.h"

PageImageAlignment::PageImageAlignment(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PageImageAlignment) {
    ui->setupUi(this);

    logicPageImageAlignment = new LogicPageImageAlignment();
    ui->quickWidget->rootContext()->setContextProperty("logicPageImageAlignment", (QObject *) logicPageImageAlignment);

    ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    ui->quickWidget->setClearColor(QColor(Qt::transparent));
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/qml/PageImageAlignment.qml")));

    mQuickItem = ui->quickWidget->rootObject();
    mQuickItem->setProperty("m_width", ui->quickWidget->width());
    mQuickItem->setProperty("m_height", ui->quickWidget->height());

    connect(logicPageImageAlignment, SIGNAL(reject()), this, SLOT(reject()));
    connect(logicPageImageAlignment, SIGNAL(accept()), this, SLOT(accept()));
}

PageImageAlignment::~PageImageAlignment() {
    delete ui;
}

LogicPageImageAlignment::LogicPageImageAlignment() {
    mReferenceImagePath = "";
    mReferenceMarkerPath = "";

    mAlignmentImagePath = "";
    mAlignmentMarkerPath = "";

#ifdef PLATFORM_WINDOWS
    mPlatformPrex = "file:///";
#else
    mPlatformPrex = "file://";
#endif
}

LogicPageImageAlignment::~LogicPageImageAlignment() {

}

void LogicPageImageAlignment::emitReject() {
    emit reject();
}

void LogicPageImageAlignment::emitAccept() {
    alignment();

    emit accept();
}

void LogicPageImageAlignment::selectReferenceImage() {
    if (selectImage(true)) {
        emit updateReferenceImage(mPlatformPrex + mReferenceImagePath);
    }
}

void LogicPageImageAlignment::selectAlignmentImage() {
    if (selectImage(false)) {
        emit updateAlignmentImage(mPlatformPrex + mAlignmentImagePath);
    }
}

bool LogicPageImageAlignment::selectImage(bool isReference) {
    QString imageFilePath = "", markerFilePath = "";

    QFileDialog fileDialog(0, isReference ? QString(tr("Open reference image")) : QString(tr("Open alignment image")));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileFilters;
    fileFilters << tr("Image files (*.bmp *.jpg *.tif)");
    QStringList suffixFilters;
    suffixFilters << ".bmp" << ".jpg" << ".tif";
    fileDialog.setNameFilters(fileFilters);

    if (fileDialog.exec() == QDialog::Accepted) {
        imageFilePath = fileDialog.selectedFiles()[0];

        if (imageFilePath.isEmpty()) {
            return false;
        }

        bool isImage = false;
        QString suffix = "";
        for (auto iter = suffixFilters.begin(); iter != suffixFilters.end(); iter++) {
            if (imageFilePath.endsWith(*iter)) {
                QString tmpFilePath = imageFilePath;
                markerFilePath = tmpFilePath.replace(*iter, ".marker");
                QFile file(markerFilePath);
                if (file.exists()) {
                    isImage = true;
                    break;
                } else {
                    QMessageBox::warning(0, tr("Warning"),
                                         tr("The marker point file with the same name as the image file does not exist!"));
                    return false;
                }
            }
        }

        if (!isImage) return false;


        if (isReference) {
            mReferenceImagePath = imageFilePath;
            mReferenceMarkerPath = markerFilePath;
        } else {
            mAlignmentImagePath = imageFilePath;
            mAlignmentMarkerPath = markerFilePath;
        }

        return true;
    }

    return false;
}

void LogicPageImageAlignment::alignment() {
    if (mReferenceImagePath.isEmpty() ||
        mAlignmentImagePath.isEmpty() ||
        mReferenceMarkerPath.isEmpty() ||
        mReferenceMarkerPath.isEmpty()) {
        return;
    }

    PspNsImgIo::Img_io io;
    io.readImg(cv::String(mReferenceImagePath.toLocal8Bit().data()));
    cv::Mat matReference = io.getSrcImg();
    io.readImg(cv::String(mAlignmentImagePath.toLocal8Bit().data()));
    cv::Mat matAlignment = io.getSrcImg();

    vector<MarkerPoint> mpReference = JsonProcess::readMarkerFromFile(mReferenceMarkerPath);
    vector<MarkerPoint> mpAlignment = JsonProcess::readMarkerFromFile(mAlignmentMarkerPath);

    vector<cv::Point2f> kpReference;
    vector<cv::Point2f> kpAlignment;
    for (auto point: mpReference) {
        kpReference.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
    }
    for (auto point: mpAlignment) {
        kpAlignment.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
    }

    cv::Mat matrix;
    PspNsAlgo::FindHomography(kpReference, kpAlignment, matrix, cv::RANSAC);

    QString logMatrix;
    logMatrix += tr("Matrix: [ ");
    logMatrix += QString::number(((double *) matrix.ptr(0))[0]) + ", ";
    logMatrix += QString::number(((double *) matrix.ptr(0))[1]) + ", ";
    logMatrix += QString::number(((double *) matrix.ptr(0))[2]) + "; ";
    logMatrix += QString::number(((double *) matrix.ptr(1))[0]) + ", ";
    logMatrix += QString::number(((double *) matrix.ptr(1))[1]) + ", ";
    logMatrix += QString::number(((double *) matrix.ptr(1))[2]) + "; ";
    logMatrix += QString::number(((double *) matrix.ptr(2))[0]) + ", ";
    logMatrix += QString::number(((double *) matrix.ptr(2))[1]) + ", ";
    logMatrix += QString::number(((double *) matrix.ptr(2))[2]) + " ]";
    PspNsBase::Log::getInstance()->writeLog(LogLevel::eInfo, logMatrix);

    cv::Mat dst;
    cv::Mat invMatrix = matrix.inv();
    cv::warpPerspective(matAlignment, dst, invMatrix, cv::Size(matAlignment.cols, matAlignment.rows));

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
    for (size_t i = 0; i < kpAlignment.size(); i++) {
        float x = kpAlignment[i].x, y = kpAlignment[i].y;
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
    for (int i = 0; i < kpAlignment.size(); i++) {
        float valX = xOffset[i] - kpReference[i].x;
        xDeviationSum += valX;

        if (fabs(valX) > xDeviationMax) {
            xDeviationMax = fabs(valX);
        }

        float valY = yOffset[i] - kpReference[i].y;
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
    xDeviationMean = xDeviationSum / (float) kpAlignment.size();
    yDeviationMean = yDeviationSum / (float) kpAlignment.size();
    disDeviationMean = disDeviationSum / (float) kpAlignment.size();
    qDebug() << "x deviation max: " << xDeviationMax << " y deviation max: " << yDeviationMax
             << " distance deviation max: " << disDeviationMax;
    qDebug() << "x deviation mean: " << xDeviationMean << " y deviation mean: " << yDeviationMean
             << " distance deviation mean: " << disDeviationMean;

    QString logDeviationMax;
    logDeviationMax += tr("Deviation max: X ");
    logDeviationMax += QString::number(xDeviationMax) + ", ";
    logDeviationMax += tr("Y ");
    logDeviationMax += QString::number(yDeviationMax) + ", ";
    logDeviationMax += tr("Distance ");
    logDeviationMax += QString::number(disDeviationMax);
    PspNsBase::Log::getInstance()->writeLog(LogLevel::eInfo, logDeviationMax);

    QString logDeviationMean;
    logDeviationMean += tr("Deviation mean: X ");
    logDeviationMean += QString::number(xDeviationMean) + ", ";
    logDeviationMean += tr("Y ");
    logDeviationMean += QString::number(yDeviationMean) + ", ";
    logDeviationMean += tr("Distance ");
    logDeviationMean += QString::number(disDeviationMean);
    PspNsBase::Log::getInstance()->writeLog(LogLevel::eInfo, logDeviationMean);

    if (SystemParam::GetInstance()->IsDrawTestImageAlignment()) {
        SystemParam::GetInstance()->AddTestInfo(logMatrix);
        SystemParam::GetInstance()->AddTestInfo(logDeviationMax);
        SystemParam::GetInstance()->AddTestInfo(logDeviationMean);
    }

    cv::Mat DivideImg;
    cv::Mat d1, d2;
    dst.convertTo(d1, CV_32FC1);
    matReference.convertTo(d2, CV_32FC1);
    PspNsAlgo::divide(d2, d1, DivideImg);

    if (SystemParam::GetInstance()->IsDebugMode()) {
        int meanVal = cv::mean(matReference)[0];
        for (int i = 0; i < matReference.rows; i++) {
            for (int j = 0; j < matReference.cols; j++) {
                if (matReference.at<ushort>(i, j) < meanVal)
                    DivideImg.at<float>(i, j) = 0;
            }
        }
    }

    double minVal, maxVal;
    cv::minMaxLoc(DivideImg, &minVal, &maxVal);

    DivideImg.convertTo(DivideImg, CV_8UC1, 255.0 / (maxVal - minVal), -255.0 * minVal / (maxVal - minVal));

    cv::Mat combonedMat(matReference.rows >= dst.rows ? matReference.rows : dst.rows,
                        matReference.cols + dst.cols + DivideImg.cols, CV_8UC1);
    combonedMat.setTo(0);

    cv::Mat left, right;
    matReference *= 1.0 / 255;
    matAlignment *= 1.0 / 255;
    matReference.convertTo(left, CV_8UC1);
    matAlignment.convertTo(right, CV_8UC1);

    left.copyTo(combonedMat(cv::Rect(0, 0, left.cols, left.rows)));
    right.copyTo(combonedMat(cv::Rect(left.cols, 0, right.cols, right.rows)));
    DivideImg.copyTo(combonedMat(cv::Rect(left.cols + right.cols, 0, DivideImg.cols, DivideImg.rows)));
    cv::cvtColor(combonedMat, combonedMat, cv::COLOR_GRAY2RGB);

    for (size_t i = 0; i < mpReference.size(); i++) {
        QPointF start(mpReference[i].x(), mpReference[i].y());
        QPointF end(mpAlignment[i].x() + left.cols, mpAlignment[i].y());

        PspNsBase::GraphicManager::GetInstance()->AddGraphic(new GraphicLine(start, end));
    }

    QStringList displayText;
    displayText << logMatrix << logDeviationMax << logDeviationMean;
    GraphicText *text = new GraphicText(QPointF(0, combonedMat.rows + 30), displayText);
    PspNsBase::GraphicManager::GetInstance()->AddGraphic(text);

    cv::cvtColor(DivideImg, DivideImg, cv::COLOR_GRAY2RGB);
    int isDisplayDivideImg = 0;
    if (isDisplayDivideImg) {
        PspNsManager::WorkImgManager::GetInstance()->setCurImg(DivideImg);
    } else {
        PspNsManager::WorkImgManager::GetInstance()->setCurImg(combonedMat);
    }
}
