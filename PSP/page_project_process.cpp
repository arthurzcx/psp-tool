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
#include "page_project_process.h"
#include "ui_page_project_process.h"
#include "manager/work_manager.h"
#include "manager/manager_doc_single_component.h"
#include "manager/manager_doc_two_component.h"
#include "algo/algo_marker_fill.h"
#include "algo/algo_filter.h"
#include "algo/algo_match.h"
#include "algo/algo_basic_arithmatic.h"
#include "algo/algo_threshold.h"
#include "algo/algo_type.h"
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "psp_utils_json_process.h"
#include <QMessageBox>
#include <QDebug>

using namespace PspNsManager;

PageProjectProcess::PageProjectProcess(QWidget *parent, PspNsManager::PspDocument *document) :
        QDialog(parent),
        ui(new Ui::PageProjectProcess),
        mDocument(document) {
    ui->setupUi(this);

    TranslateUi();
}

void PageProjectProcess::TranslateUi() {
    ui->checkBoxBackgroundImageSubtract->setText(tr("Background image substraction"));
    ui->checkBoxFlatFieldCorrection->setText(tr("Flat field correction"));
    ui->checkBoxFillUpMarkers->setText(tr("Fill up markers"));
    ui->checkBoxThresholdWork->setText(tr("Threshold work"));
    ui->checkBoxDistortionCorrection->setText(tr("Distortion correction"));
    ui->checkBoxAlignmentImages->setText(tr("Alignment images"));
    ui->checkBoxImageConvert->setText(tr("Image convert"));
    ui->checkBoxFinalFiltering->setText(tr("Final filtering"));
}

PageProjectProcess::~PageProjectProcess() {
    delete ui;
}

bool SingleComponentDocumentProcess(PspNsManager::PspDocumentSingleComponent *document) {
    const PspNsManager::PspDocumentProcessConfig &config = document->GetProcessConfig();

    cv::Mat tmpBackgroundImgWindOn = document->GetBackgroundImg();
    cv::Mat tmpBackgroundImgWindOff = document->GetBackground2Img();
    cv::Mat tmpWindOnImg = document->GetWindOnImg().clone();
    if (tmpWindOnImg.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-on image is empty!"));
        msgBox.exec();
        return false;
    }
    cv::Mat tmpWindOffImg = document->GetWindOffImg().clone();
    if (tmpWindOffImg.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-off image is empty!"));
        msgBox.exec();
        return false;
    }

    const std::vector<MarkerPoint> &mpWindOn = document->GetWindOnMarkers();
    if (mpWindOn.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-on marker file is empty"));
        msgBox.exec();
        return false;
    }
    const std::vector<MarkerPoint> &mpWindOff = document->GetWindOffMarkers();
    if (mpWindOff.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-off maker file is empty"));
        msgBox.exec();
        return false;
    }

    if (config.mIsBackgroundImageSubtract) {
        if (tmpBackgroundImgWindOn.empty()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Warning: The background (wind on) image is empty"));
            msgBox.exec();
            return false;
        } else {
            PspNsAlgo::subtract(tmpWindOnImg, tmpBackgroundImgWindOn, tmpWindOnImg);
        }

        if (tmpBackgroundImgWindOff.empty()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Warning: The background (wind off) image is empty"));
            msgBox.exec();
            return false;
        } else {
            PspNsAlgo::subtract(tmpWindOffImg, tmpBackgroundImgWindOff, tmpWindOffImg);
        }
    }

    if (config.mIsFlatFieldCorrection) {

    }

    if (config.mIsFillUpMarkers) {
        qDebug() << "Fillup markers" << config.mIsFillUpMarkers;
        PspNsAlgo::FillupMarkers(tmpWindOnImg, mpWindOn);
        PspNsAlgo::FillupMarkers(tmpWindOffImg, mpWindOff);
    }

    if (config.mIsThresholdWork) {
        double lowThresholdWindOn, highThresholdWindOn;
        PspNsAlgo::calcAdapiveThreshold(tmpWindOnImg, lowThresholdWindOn, highThresholdWindOn);
        PspNsAlgo::thresholdSegmentation(tmpWindOnImg, lowThresholdWindOn);

        double lowThresholdWindOff, highThresholdWindOff;
        PspNsAlgo::calcAdapiveThreshold(tmpWindOffImg, lowThresholdWindOff, highThresholdWindOff);
        PspNsAlgo::thresholdSegmentation(tmpWindOffImg, lowThresholdWindOff);
    }

    if (config.mIsDistortionCorrection) {

    }

    if (config.mIsAlignmentImages) {
        vector<cv::Point2f> kpWindOff, kpWindOn;
        for (auto point: mpWindOff) {
            kpWindOff.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
        }
        for (auto point: mpWindOn) {
            kpWindOn.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
        }

        cv::Mat matrix;
        PspNsAlgo::FindHomography(kpWindOff, kpWindOn, matrix, cv::RANSAC);

        cv::Mat alignedImg;
        cv::Mat invMatrix = matrix.inv();
        cv::warpPerspective(tmpWindOnImg, tmpWindOnImg, invMatrix,
                            cv::Size(tmpWindOnImg.cols, tmpWindOnImg.rows));

        vector<float> xOffset, yOffset;
        for (size_t i = 0; i < kpWindOn.size(); i++) {
            float x = kpWindOn[i].x, y = kpWindOn[i].y;
            float t = x * ((double *) invMatrix.ptr(2))[0] + y * ((double *) invMatrix.ptr(2))[1] +
                      ((double *) invMatrix.ptr(2))[2];
            xOffset.push_back((x * ((double *) invMatrix.ptr(0))[0] + y * ((double *) invMatrix.ptr(0))[1] +
                               ((double *) invMatrix.ptr(0))[2]) / t);
            yOffset.push_back((x * ((double *) invMatrix.ptr(1))[0] + y * ((double *) invMatrix.ptr(1))[1] +
                               ((double *) invMatrix.ptr(1))[2]) / t);
        }

        double xDeviationMax = 0.0, xDeviationMean = 0.0, xDeviationSum = 0.0;
        double yDeviationMax = 0.0, yDeviationMean = 0.0, yDeviationSum = 0.0;
        double disDeviationMax = 0.0, disDeviationMean = 0.0, disDeviationSum = 0.0;
        for (int i = 0; i < kpWindOn.size(); i++) {
            float valX = xOffset[i] - kpWindOff[i].x;
            xDeviationSum += valX;

            if (fabs(valX) > xDeviationMax) {
                xDeviationMax = fabs(valX);
            }

            float valY = yOffset[i] - kpWindOff[i].y;
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
        xDeviationMean = xDeviationSum / (float) kpWindOn.size();
        yDeviationMean = yDeviationSum / (float) kpWindOn.size();
        disDeviationMean = disDeviationSum / (float) kpWindOn.size();

        document->SetDeviationMaxX(xDeviationMax);
        document->SetDeviationMaxY(yDeviationMax);
        document->SetDeviationMaxDistance(disDeviationMax);
        document->SetDeviationMeanX(xDeviationMean);
        document->SetDeviationMeanY(yDeviationMean);
        document->SetDeviationMeanDistance(disDeviationMean);
    }

    if (config.mIsImageConvert) {
        cv::Mat divideImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);
        tmpWindOffImg.convertTo(tmpWindOffImg, CV_32FC1);
        tmpWindOnImg.convertTo(tmpWindOnImg, CV_32FC1);

        PspNsAlgo::divide32F(tmpWindOffImg, tmpWindOnImg, divideImg);

        if (config.mIsThresholdWork) {
            for (int i = 0; i < divideImg.rows; i++) {
                for (int j = 0; j < divideImg.cols; j++) {
                    if (tmpWindOffImg.at<float>(i, j) == 0) {
                        divideImg.at<float>(i, j) = 0;
                    }
                }
            }
        }

        document->SetLightRatioImg(divideImg);

        Json j;
        if (!JsonProcess::GetInstance()->readJsonFile(document->GetPathPaintCalibrationFile(), j)) {
            QMessageBox msg;
            msg.setText(QObject::tr("Warning: can't read paint calibration file!"));
            msg.exec();
            return false;
        }

        double g00 = j["0"], g10 = j["1"], g20 = j["2"],
                g01 = j["3"], g11 = j["4"], g21 = j["5"],
                g02 = j["6"], g12 = j["7"], g22 = j["8"];

        cv::Mat pressureViewImg(divideImg.rows, divideImg.cols, CV_32FC1);
        divideImg.copyTo(pressureViewImg);

        double T = document->GetTestFeaturesWindOnTemperature();
        eUnitTemperature unit1 = PspNsBase::TemperatureFromString(
                QString::fromStdString(j["temperature_unit"]));
        T = PspNsBase::TemperatureConvert(T, document->GetTestFeaturesTemperatureUnit(), unit1);

        bool ReferenceConditonCorrection = true;
        double Ir0;
        if (ReferenceConditonCorrection) {
            double Tref = document->GetTestFeaturesWindOffTemperature();
            Tref = PspNsBase::TemperatureConvert(Tref, document->GetTestFeaturesTemperatureUnit(), unit1);
            double Pref = document->GetTestFeaturesWindOffPressure();
            eUnitPressure unitP0 = document->GetTestFeaturesPressureUnit();
            eUnitPressure unitP1 = PspNsBase::PressureFromString(
                    QString::fromStdString(j["pressure_unit"]));
            Pref = PressureConvert(Pref, unitP0, unitP1);
            double a = g20 + g21 * Tref + g22 * pow(Tref, 2);
            double b = g10 + g11 * Tref + g12 * pow(Tref, 2);
            double c = g00 + g01 * Tref + g02 * pow(Tref, 2) - Pref;
            Ir0 = 0.5 * (-b + sqrt(b * b - 4 * a * c)) / a;
            qDebug() << "Windff T" << Tref << "Windoff P" << Pref << "Ir0" << Ir0;
            qDebug() << "a" << a << "b" << b << "c" << c << "Ir0" << Ir0;
            if (abs(Ir0 - 1) > 0.07) {
                QMessageBox msg;
                msg.setText(QObject::tr("Warning:Please check the Pref/Tref of the wind tunnel test and calibration"));
                msg.exec();
            }
        } else {
            Ir0 = 1;
        }

        for (int i = 0; i < divideImg.rows; i++) {
            for (int j = 0; j < divideImg.cols; j++) {
                double I = divideImg.at<float>(i, j);
                I = I * Ir0;
                pressureViewImg.at<float>(i, j) = (float) (g00 + g10 * I + g20 * pow(I, 2) +
                                                           g01 * T + g11 * I * T + g21 * pow(I, 2) * T +
                                                           g02 * pow(T, 2) + g12 * I * pow(T, 2) +
                                                           g22 * pow(I, 2) * pow(T, 2));
            }
        }
        if (config.mIsThresholdWork) {
            for (int i = 0; i < divideImg.rows; i++) {
                for (int j = 0; j < divideImg.cols; j++) {
                    if (tmpWindOffImg.at<float>(i, j) == 0) {
                        pressureViewImg.at<float>(i, j) = 0;
                    }
                }
            }
        }
        document->SetPressureImg(pressureViewImg);

        double staticPressure = document->GetTestFeaturesStaticPressure();
        double dynamicPressure = document->GetTestFeaturesDynamicPressure();

        eUnitPressure unitP1 = PspNsBase::PressureFromString(
                QString::fromStdString(j["pressure_unit"]));
        eUnitPressure unitP2 = document->GetTestFeaturesPressureUnit();
        staticPressure = PressureConvert(staticPressure, unitP2, unitP1);
        dynamicPressure = PressureConvert(dynamicPressure, unitP2, unitP1);

        cv::Mat cpImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);
        pressureViewImg.copyTo(cpImg);
        qDebug() << staticPressure << dynamicPressure;

        for (int i = 0; i < pressureViewImg.rows; i++) {
            for (int j = 0; j < pressureViewImg.cols; j++) {
                if (pressureViewImg.at<float>(i, j) > 0) {
                    double p = pressureViewImg.at<float>(i, j);
                    cpImg.at<float>(i, j) = (p - staticPressure) / dynamicPressure;
                }
            }
        }
        document->SetPressureCoefficientImg(cpImg);
    }

    if (config.mIsFinalFiltering) {
        cv::Mat lightRatioImg = document->GetLightRatioImg();
        cv::Mat pressureViewImg = document->GetPressureImg();
        cv::Mat cpImg = document->GetPressureCoefficientImg();

        PspNsAlgo::filterMedian(lightRatioImg, lightRatioImg, 3);
        PspNsAlgo::filterGaussian(lightRatioImg, lightRatioImg, cv::Size(3, 3), 10);
        PspNsAlgo::filterMedian(pressureViewImg, pressureViewImg, 3);
        PspNsAlgo::filterGaussian(pressureViewImg, pressureViewImg, cv::Size(3, 3), 10);
        PspNsAlgo::filterMedian(cpImg, cpImg, 3);
        PspNsAlgo::filterGaussian(cpImg, cpImg, cv::Size(3, 3), 10);

        document->SetLightRatioImg(lightRatioImg);
        document->SetPressureImg(pressureViewImg);
        document->SetPressureCoefficientImg(cpImg);
    }

    PspNsManager::WorkImgManager::GetInstance()->setCurImg(document->GetPressureImg());

    return true;
}

bool DualComponentDocumentProcess(PspNsManager::PspDocumentTwoComponent *document) {
    const PspNsManager::PspDocumentProcessConfig &config = document->GetProcessConfig();

    cv::Mat tmpBackgroundImg = document->GetBackgroundImg();
    cv::Mat tmpBackground2Img = document->GetBackground2Img();
    cv::Mat tmpBackground3Img = document->GetBackground3Img();
    cv::Mat tmpBackground4Img = document->GetBackground4Img();

    cv::Mat tmpWindOnImg = document->GetWindOnImg().clone();
    if (tmpWindOnImg.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-on image is empty!"));
        msgBox.exec();
        return false;
    }

    cv::Mat tmpWindOffImg = document->GetWindOffImg().clone();
    if (tmpWindOffImg.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-off image is empty!"));
        msgBox.exec();
        return false;
    }

    cv::Mat tmpWindOn2Img = document->GetWindOn2Img().clone();
    if (tmpWindOn2Img.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-on 2 image is empty!"));
        msgBox.exec();
        return false;
    }

    cv::Mat tmpWindOff2Img = document->GetWindOff2Img().clone();
    if (tmpWindOff2Img.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-off 2 image is empty!"));
        msgBox.exec();
        return false;
    }

    const std::vector<MarkerPoint> &mpWindOn = document->GetWindOnMarkers();
    if (mpWindOn.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-on marker file is empty"));
        msgBox.exec();
        return false;
    }
    const std::vector<MarkerPoint> &mpWindOff = document->GetWindOffMarkers();
    if (mpWindOff.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-off marker file is empty"));
        msgBox.exec();
        return false;
    }

    const std::vector<MarkerPoint> &mpWindOn2 = document->GetWindOn2Markers();
    if (mpWindOn2.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-on 2 marker file is empty"));
        msgBox.exec();
        return false;
    }
    const std::vector<MarkerPoint> &mpWindOff2 = document->GetWindOff2Markers();
    if (mpWindOff2.empty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The wind-off 2 marker file is empty"));
        msgBox.exec();
        return false;
    }

    cv::Mat refeDivideImg(tmpWindOn2Img.rows, tmpWindOn2Img.cols, CV_32F);
    cv::Mat psensDivideImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);
    cv::Mat divideImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);

    if (config.mIsBackgroundImageSubtract) {
        if (tmpBackgroundImg.empty()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Warning: The background image (paint-sensitive, wind on) is empty"));
            msgBox.exec();
            return false;
        }
        if (tmpBackground2Img.empty()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Warning: The background image (paint-sensitive, wind off) is empty"));
            msgBox.exec();
            return false;
        }
        if (tmpBackground3Img.empty()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Warning: The background image (reference, wind on) is empty"));
            msgBox.exec();
            return false;
        }
        if (tmpBackground4Img.empty()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Warning: The background image (reference, wind off) is empty"));
            msgBox.exec();
            return false;
        }
        PspNsAlgo::subtract(tmpWindOnImg, tmpBackgroundImg, tmpWindOnImg);
        PspNsAlgo::subtract(tmpWindOffImg, tmpBackground2Img, tmpWindOffImg);

        PspNsAlgo::subtract(tmpWindOn2Img, tmpBackground3Img, tmpWindOn2Img);
        PspNsAlgo::subtract(tmpWindOff2Img, tmpBackground4Img, tmpWindOff2Img);
    }

    if (config.mIsFlatFieldCorrection) {

    }

    if (config.mIsFillUpMarkers) {
        PspNsAlgo::FillupMarkers(tmpWindOnImg, mpWindOn);
        PspNsAlgo::FillupMarkers(tmpWindOffImg, mpWindOff);

        PspNsAlgo::FillupMarkers(tmpWindOn2Img, mpWindOn2);
        PspNsAlgo::FillupMarkers(tmpWindOff2Img, mpWindOff2);
    }

    if (config.mIsThresholdWork) {
        double lowThresholdWindOn, highThresholdWindOn;
        PspNsAlgo::calcAdapiveThreshold(tmpWindOnImg, lowThresholdWindOn, highThresholdWindOn);
        PspNsAlgo::thresholdSegmentation(tmpWindOnImg, lowThresholdWindOn);

        double lowThresholdWindOff, highThresholdWindOff;
        PspNsAlgo::calcAdapiveThreshold(tmpWindOffImg, lowThresholdWindOff, highThresholdWindOff);
        PspNsAlgo::thresholdSegmentation(tmpWindOffImg, lowThresholdWindOff);

        double lowThresholdWindOn2, highThresholdWindOn2;
        PspNsAlgo::calcAdapiveThreshold(tmpWindOn2Img, lowThresholdWindOn2, highThresholdWindOn2);
        PspNsAlgo::thresholdSegmentation(tmpWindOn2Img, lowThresholdWindOn2);

        double lowThresholdWindOff2, highThresholdWindOff2;
        PspNsAlgo::calcAdapiveThreshold(tmpWindOff2Img, lowThresholdWindOff2, highThresholdWindOff2);
        PspNsAlgo::thresholdSegmentation(tmpWindOff2Img, lowThresholdWindOff2);
    }

    if (config.mIsDistortionCorrection) {

    }

    if (config.mIsAlignmentImages) {
        vector<cv::Point2f> kpWindOff, kpWindOn;
        for (auto point: mpWindOff) {
            kpWindOff.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
        }
        for (auto point: mpWindOn) {
            kpWindOn.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
        }

        cv::Mat matrix;
        PspNsAlgo::FindHomography(kpWindOff, kpWindOn, matrix, cv::RANSAC);

        cv::Mat alignedImg;
        cv::Mat invMatrix = matrix.inv();
        cv::warpPerspective(tmpWindOnImg, tmpWindOnImg, invMatrix,
                            cv::Size(tmpWindOnImg.cols, tmpWindOnImg.rows));

        vector<cv::Point2f> kpWindOff2, kpWindOn2;
        for (auto point: mpWindOff2) {
            kpWindOff2.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
        }
        for (auto point: mpWindOn2) {
            kpWindOn2.push_back(cv::Point2f(point.getPoint().x(), point.getPoint().y()));
        }

        PspNsAlgo::FindHomography(kpWindOff2, kpWindOn2, matrix, cv::RANSAC);

        invMatrix = matrix.inv();
        cv::warpPerspective(tmpWindOn2Img, tmpWindOn2Img, invMatrix,
                            cv::Size(tmpWindOn2Img.cols, tmpWindOn2Img.rows));

        PspNsAlgo::FindHomography(kpWindOff, kpWindOff2, matrix, cv::RANSAC);
        invMatrix = matrix.inv();
        cv::warpPerspective(refeDivideImg, refeDivideImg, invMatrix,
                            cv::Size(tmpWindOn2Img.cols, tmpWindOn2Img.rows));
        cv::warpPerspective(tmpWindOff2Img, tmpWindOff2Img, invMatrix,
                            cv::Size(tmpWindOn2Img.cols, tmpWindOn2Img.rows));
        cv::warpPerspective(tmpWindOn2Img, tmpWindOn2Img, invMatrix,
                            cv::Size(tmpWindOn2Img.cols, tmpWindOn2Img.rows));

        vector<float> xOffset, yOffset;
        for (size_t i = 0; i < kpWindOff2.size(); i++) {
            float x = kpWindOff2[i].x, y = kpWindOff2[i].y;
            float t = x * ((double *) invMatrix.ptr(2))[0] + y * ((double *) invMatrix.ptr(2))[1] +
                      ((double *) invMatrix.ptr(2))[2];
            xOffset.push_back((x * ((double *) invMatrix.ptr(0))[0] + y * ((double *) invMatrix.ptr(0))[1] +
                               ((double *) invMatrix.ptr(0))[2]) / t);
            yOffset.push_back((x * ((double *) invMatrix.ptr(1))[0] + y * ((double *) invMatrix.ptr(1))[1] +
                               ((double *) invMatrix.ptr(1))[2]) / t);
        }

        double xDeviationMax = 0.0, xDeviationMean = 0.0, xDeviationSum = 0.0;
        double yDeviationMax = 0.0, yDeviationMean = 0.0, yDeviationSum = 0.0;
        double disDeviationMax = 0.0, disDeviationMean = 0.0, disDeviationSum = 0.0;
        for (int i = 0; i < kpWindOn2.size(); i++) {
            float valX = xOffset[i] - kpWindOff[i].x;
            xDeviationSum += valX;

            if (fabs(valX) > xDeviationMax) {
                xDeviationMax = fabs(valX);
            }

            float valY = yOffset[i] - kpWindOff[i].y;
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
        xDeviationMean = xDeviationSum / (float) kpWindOn.size();
        yDeviationMean = yDeviationSum / (float) kpWindOn.size();
        disDeviationMean = disDeviationSum / (float) kpWindOn.size();

        document->SetDeviationMaxX(xDeviationMax);
        document->SetDeviationMaxY(yDeviationMax);
        document->SetDeviationMaxDistance(disDeviationMax);
        document->SetDeviationMeanX(xDeviationMean);
        document->SetDeviationMeanY(yDeviationMean);
        document->SetDeviationMeanDistance(disDeviationMean);
    }

    if (config.mIsImageConvert) {
        tmpWindOnImg.convertTo(tmpWindOnImg, CV_32FC1);
        tmpWindOn2Img.convertTo(tmpWindOn2Img, CV_32FC1);
        tmpWindOffImg.convertTo(tmpWindOffImg, CV_32FC1);
        tmpWindOff2Img.convertTo(tmpWindOff2Img, CV_32FC1);

        cv::Mat PsensDivideImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);
        cv::Mat RefeDivideImg(tmpWindOn2Img.rows, tmpWindOn2Img.cols, CV_32F);
        cv::Mat divideImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);

        PspNsAlgo::divide32F(tmpWindOffImg, tmpWindOnImg, PsensDivideImg);
        PspNsAlgo::divide32F(tmpWindOff2Img, tmpWindOn2Img, RefeDivideImg);
        PspNsAlgo::divide32F(PsensDivideImg, RefeDivideImg, divideImg);

        if (config.mIsThresholdWork) {
            for (int i = 0; i < divideImg.rows; i++) {
                for (int j = 0; j < divideImg.cols; j++) {
                    if (tmpWindOffImg.at<float>(i, j) == 0) {
                        divideImg.at<float>(i, j) = 0;
                    }
                }
            }
        }

        document->SetLightRatioImg(divideImg);

        Json j;
        if (!JsonProcess::GetInstance()->readJsonFile(document->GetPathPaintCalibrationFile(), j)) {
            QMessageBox msg;
            msg.setText(QObject::tr("Warning: can't read paint calibration file!"));
            msg.exec();
            return false;
        }

        double g00 = j["0"], g10 = j["1"], g20 = j["2"],
                g01 = j["3"], g11 = j["4"], g21 = j["5"],
                g02 = j["6"], g12 = j["7"], g22 = j["8"];

        eUnitTemperature unit1 = PspNsBase::TemperatureFromString(
                QString::fromStdString(j["temperature_unit"]));
        eUnitPressure unitP1 = PspNsBase::PressureFromString(
                QString::fromStdString(j["pressure_unit"]));

        cv::Mat pressureViewImg(divideImg.rows, divideImg.cols, CV_32FC1);
        divideImg.copyTo(pressureViewImg);

        double T = document->GetTestFeaturesWindOnTemperature();
        T = PspNsBase::TemperatureConvert(T, document->GetTestFeaturesTemperatureUnit(), unit1);

        bool ReferenceConditonCorrection = true;
        double Ir0;
        if (ReferenceConditonCorrection) {
            double Tref = document->GetTestFeaturesWindOffTemperature();
            Tref = PspNsBase::TemperatureConvert(Tref, document->GetTestFeaturesTemperatureUnit(), unit1);
            double Pref = document->GetTestFeaturesWindOffPressure();
            eUnitPressure unitP0 = document->GetTestFeaturesPressureUnit();
            Pref = PressureConvert(Pref, unitP0, unitP1);
            double a = g20 + g21 * Tref + g22 * pow(Tref, 2);
            double b = g10 + g11 * Tref + g12 * pow(Tref, 2);
            double c = g00 + g01 * Tref + g02 * pow(Tref, 2) - Pref;
            Ir0 = 0.5 * (-b + sqrt(b * b - 4 * a * c)) / a;

            if (abs(Ir0 - 1) > 0.07) {
                QMessageBox msg;
                msg.setText(QObject::tr("Warning:Please check the Pref/Tref of the wind tunnel test and calibration"));
                msg.exec();
            }
        } else {
            Ir0 = 1;
        }

        for (int i = 0; i < divideImg.rows; i++) {
            for (int j = 0; j < divideImg.cols; j++) {
                double I = divideImg.at<float>(i, j);
                I = I * Ir0;
                pressureViewImg.at<float>(i, j) = (float) (g00 + g10 * I + g20 * pow(I, 2) +
                                                           g01 * T + g11 * I * T + g21 * pow(I, 2) * T +
                                                           g02 * pow(T, 2) + g12 * I * pow(T, 2) +
                                                           g22 * pow(I, 2) * pow(T, 2));
            }
        }

        if (config.mIsThresholdWork) {
            for (int i = 0; i < divideImg.rows; i++) {
                for (int j = 0; j < divideImg.cols; j++) {
                    if (tmpWindOffImg.at<float>(i, j) == 0) {
                        pressureViewImg.at<float>(i, j) = 0;
                    }
                }
            }
        }
        document->SetPressureImg(pressureViewImg);

        double staticPressure = document->GetTestFeaturesStaticPressure();
        double dynamicPressure = document->GetTestFeaturesDynamicPressure();

        eUnitPressure unitP2 = document->GetTestFeaturesPressureUnit();
        staticPressure = PressureConvert(staticPressure, unitP2, unitP1);
        dynamicPressure = PressureConvert(dynamicPressure, unitP2, unitP1);

        cv::Mat cpImg(tmpWindOnImg.rows, tmpWindOnImg.cols, CV_32F);
        pressureViewImg.copyTo(cpImg);

        for (int i = 0; i < pressureViewImg.rows; i++) {
            for (int j = 0; j < pressureViewImg.cols; j++) {
                if (pressureViewImg.at<float>(i, j) > 0) {
                    double p = pressureViewImg.at<float>(i, j);
                    cpImg.at<float>(i, j) = (p - staticPressure) / dynamicPressure;
                }
            }
        }

        document->SetPressureCoefficientImg(cpImg);
    }

    if (config.mIsFinalFiltering) {
        cv::Mat lightRatioImg = document->GetLightRatioImg();
        cv::Mat pressureViewImg = document->GetPressureImg();
        cv::Mat cpImg = document->GetPressureCoefficientImg();

        PspNsAlgo::filterMedian(lightRatioImg, lightRatioImg, 3);
        PspNsAlgo::filterGaussian(lightRatioImg, lightRatioImg, cv::Size(3, 3), 10);
        PspNsAlgo::filterMedian(pressureViewImg, pressureViewImg, 3);
        PspNsAlgo::filterGaussian(pressureViewImg, pressureViewImg, cv::Size(3, 3), 10);
        PspNsAlgo::filterMedian(cpImg, cpImg, 3);
        PspNsAlgo::filterGaussian(cpImg, cpImg, cv::Size(3, 3), 10);

        document->SetLightRatioImg(lightRatioImg);
        document->SetPressureImg(pressureViewImg);
        document->SetPressureCoefficientImg(cpImg);
    }

    PspNsManager::WorkImgManager::GetInstance()->setCurImg(document->GetPressureImg());

    return true;
}

void PageProjectProcess::accept() {
    PspNsManager::PspDocumentProcessConfig config;
    config.mIsBackgroundImageSubtract = ui->checkBoxBackgroundImageSubtract->isChecked();
    config.mIsFlatFieldCorrection = ui->checkBoxFlatFieldCorrection->isChecked();
    config.mIsFillUpMarkers = ui->checkBoxFillUpMarkers->isChecked();
    config.mIsThresholdWork = ui->checkBoxThresholdWork->isChecked();
    config.mIsDistortionCorrection = ui->checkBoxDistortionCorrection->isChecked();
    config.mIsAlignmentImages = ui->checkBoxAlignmentImages->isChecked();
    config.mIsImageConvert = ui->checkBoxImageConvert->isChecked();
    config.mIsFinalFiltering = ui->checkBoxFinalFiltering->isChecked();

    WorkImgManager::GetInstance()->setCurImg(cv::Mat());

    if (PspNsManager::PspDocumentTwoComponent *document = dynamic_cast<PspNsManager::PspDocumentTwoComponent *>(mDocument)) {
        document->ResetConfig();
        document->ResetResult();
        document->SetProcessConfig(config);

        bool bRet = DualComponentDocumentProcess(document);
        if (!bRet) {

        }
    } else if (PspNsManager::PspDocumentSingleComponent *document = dynamic_cast<PspNsManager::PspDocumentSingleComponent *>(mDocument)) {
        document->ResetConfig();
        document->ResetResult();

        document->SetProcessConfig(config);

        bool bRet = SingleComponentDocumentProcess(document);
        if (!bRet) {
        }
    }

    QDialog::accept();
}
