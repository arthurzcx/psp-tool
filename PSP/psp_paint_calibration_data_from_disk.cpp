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
#include "psp_paint_calibration_data_from_disk.h"
#include "io/io.h"
#include "algo/algo_basic_arithmatic.h"
#include <QFileDialog>

PaintCalibrationDataFromDisk::PaintCalibrationDataFromDisk()
        : QObject(), mQmlEngine(nullptr), mDocument(nullptr) {
#ifdef PLATFORM_WINDOWS
    mPlatformPrex = "file:///";
#else
    mPlatformPrex = "file://";
#endif
}

PaintCalibrationDataFromDisk *PaintCalibrationDataFromDisk::GetInstance() {
    static PaintCalibrationDataFromDisk pagePaintCalibrationDataFromDisk;
    return &pagePaintCalibrationDataFromDisk;
}

void PaintCalibrationDataFromDisk::Init(PspNsManager::PspDocPaintCalibration *document) {
    if (!document) return;
    mDocument = document;

    ReleaseMemory();

    mQmlEngine = new QQmlApplicationEngine();
    mQmlEngine->rootContext()->setContextProperty("dataUtils", static_cast<QObject *>(this));
    mQmlEngine->load(QUrl(QStringLiteral("qrc:/qml/PagePaintCalibrationDataFromDisk.qml")));

    QQuickWindow *window = qobject_cast<QQuickWindow *>(mQmlEngine->rootObjects().at(0));
    if (window) {
        window->show();
    }
}

PaintCalibrationDataFromDisk::~PaintCalibrationDataFromDisk() {
}

void PaintCalibrationDataFromDisk::ReleaseMemory() {
    if (mQmlEngine) {
        delete mQmlEngine;
        mQmlEngine = nullptr;
    }
}

QString PaintCalibrationDataFromDisk::getModel() {
    if (mDocument) {
        Json j;

        for (auto iter = mDocument->GetData().begin(); iter != mDocument->GetData().end(); ++iter) {
            Json element;
            QFileInfo fileInfo((*iter).mImagePath);
            element["BaseName"] = fileInfo.fileName().toStdString();
            element["ImagePath"] = (mPlatformPrex + (*iter).mImagePath).toStdString();
            element["Pressure"] = (*iter).mPressure;
            element["Temperature"] = (*iter).mTemperature;
            element["isBackground"] = (*iter).mIsBackground;
            element["isReference"] = (*iter).mIsReference;
            j.push_back(element);
        }

        return QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));
    }

    return QString("");
}

QString PaintCalibrationDataFromDisk::selectDirectory() {
    QString dirPath = QFileDialog::getExistingDirectory(nullptr, tr("Select directory"));
    if (dirPath.isEmpty()) return QString("");

    Json j;

    QDir dir(dirPath);
    dir.setFilter(QDir::Files);

    QFileInfoList fileList = dir.entryInfoList();

    for (int index = 0; index < fileList.size(); index++) {
        QFileInfo fileInfo = fileList.at(index);

        if (fileInfo.fileName().endsWith(".tif")) {
            Json element;
            element["BaseName"] = fileInfo.fileName().toStdString();
            element["ImagePath"] = QString(mPlatformPrex + dirPath + "/" + fileInfo.fileName()).toStdString();
            element["Pressure"] = 0.0;
            element["Temperature"] = 0.0;
            element["isBackground"] = false;
            element["isReference"] = false;
            j.push_back(element);
        }
    }

    if (j.is_null()) {
        return QString("");
    } else {
        return QString::fromStdString(j.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));
    }
}

void PaintCalibrationDataFromDisk::setModelFormQml(QString model) {
    PspNsManager::PspTestFeaturesPaintCalibration feature = mDocument->GetTestFeatures();

    Json j = Json::parse(model.toStdString());

    std::vector<PspNsManager::PspDataPaintCalibration> data;

    for (auto iter = j.begin(); iter != j.end(); ++iter) {
        PspNsManager::PspDataPaintCalibration tmpData;
        tmpData.mImagePath = QString::fromStdString((*iter)["ImagePath"]).replace(mPlatformPrex, "");
        tmpData.mPressure = (double) (*iter)["Pressure"];
        tmpData.mTemperature = (double) (*iter)["Temperature"];
        tmpData.mIsBackground = (*iter)["isBackground"];
        tmpData.mIsReference = (*iter)["isReference"];

        if (tmpData.mIsReference) {
            feature.mPressureNormalizing = tmpData.mPressure;
            feature.mTemperatureNormalizing = tmpData.mTemperature;
        }

        data.push_back(tmpData);
    }

    mDocument->SetData(data);
    mDocument->SetTestFeatures(feature);

    ProcessDocumentPaintCalibration(mDocument);
}

bool ProcessDocumentPaintCalibration(PspNsManager::PspDocPaintCalibration *document) {
    cv::Mat backgroundImg, referenceImg;
    std::vector<cv::Mat> generalImags;
    std::vector<double> pressures, temperatures;

    const PspNsBase::GraphicRect &roi = document->GetRoi();
    float top = roi.GetTop();
    float left = roi.GetLeft();
    float bottom = roi.GetRect().bottom();
    float right = roi.GetRect().right();
    cv::Rect2f rt(left, top, right - left, bottom - top);

    const std::vector<PspNsManager::PspDataPaintCalibration> &data = document->GetData();

    PspNsImgIo::Img_io io;
    for (auto iter = data.begin(); iter != data.end(); ++iter) {
        PspNsManager::PspDataPaintCalibration tmp = *iter;

        if (tmp.mIsBackground) {
            io.readImg(tmp.mImagePath.toStdString());
            backgroundImg = io.getSrcImg();

            if (backgroundImg.empty()) return false;

            if (roi.IsValid(PspNsBase::GraphicRect(0, 0, backgroundImg.cols, backgroundImg.rows))) {
                backgroundImg = io.getSrcImg()(rt);
            }
        } else if (tmp.mIsReference) {
            io.readImg(tmp.mImagePath.toStdString());

            referenceImg = io.getSrcImg();
            if (referenceImg.empty()) return false;

            if (roi.IsValid(PspNsBase::GraphicRect(0, 0, referenceImg.cols, referenceImg.rows))) {
                referenceImg = io.getSrcImg()(rt);
            }

            generalImags.push_back(referenceImg);

            pressures.push_back(tmp.mPressure);
            temperatures.push_back(tmp.mTemperature);
        } else {
            io.readImg(tmp.mImagePath.toStdString());

            cv::Mat tmpMat = io.getSrcImg();
            if (tmpMat.empty()) return false;
            if (roi.IsValid(PspNsBase::GraphicRect(0, 0, tmpMat.cols, tmpMat.rows))) {
                tmpMat = io.getSrcImg()(rt);
            }

            generalImags.push_back(tmpMat);

            pressures.push_back(tmp.mPressure);
            temperatures.push_back(tmp.mTemperature);
        }
    }

    if (backgroundImg.empty() || referenceImg.empty()) return false;

    if (generalImags.size() != pressures.size() || generalImags.size() != temperatures.size()) {
        return false;
    }

    cv::Mat referenceMinusImg = referenceImg - backgroundImg;

    std::vector<double> ratios;
    std::vector<double> maxVals;
    std::vector<double> minVals;
    std::vector<double> stdVals;

    for (size_t i = 0; i < generalImags.size(); i++) {
        cv::Mat generalMinuxImg = generalImags[i] - backgroundImg;
        cv::Mat ratioImg(referenceMinusImg.rows, referenceMinusImg.cols, CV_32F);
        referenceMinusImg.convertTo(referenceMinusImg, CV_32FC1);
        generalMinuxImg.convertTo(generalMinuxImg, CV_32FC1);
        PspNsAlgo::divide32F(referenceMinusImg, generalMinuxImg, ratioImg);

        cv::medianBlur(ratioImg, ratioImg, 5);
        cv::GaussianBlur(ratioImg, ratioImg, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

        cv::Scalar mean;
        cv::Scalar stddev;
        cv::meanStdDev(ratioImg, mean, stddev);
        double meanValue = mean.val[0];
        double stdValue = stddev.val[0];
        ratios.push_back(meanValue);
        stdVals.push_back(stdValue);

        double maxx = 0, minn = 0;
        double *max = &maxx;
        double *min = &minn;
        minMaxIdx(ratioImg, min, max);
        maxVals.push_back(maxx);
        minVals.push_back(minn);
    }

    QString appPath = QCoreApplication::applicationDirPath();
    QString fileTmpPath = appPath + "/paintcalibrationTmp";

    double maxPressure = -DBL_MAX, minPressure = DBL_MAX, maxTemperture = -DBL_MAX, minTemperature = DBL_MAX;

    double torlenceTemperature = document->GetTestFeatures().mTemperatureTolerance;

    QFile file(fileTmpPath);
    if (file.exists()) {
        file.remove();
    }
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        for (size_t i = 0; i < ratios.size(); i++) {
            maxPressure = fmax(maxPressure, pressures[i]);
            minPressure = fmin(minPressure, pressures[i]);
            maxTemperture = fmax(maxTemperture, temperatures[i]);
            minTemperature = fmin(minTemperature, temperatures[i]);

            file.write(QString::number(pressures[i]).toStdString().c_str());
            file.write("\t");
            file.write(QString::number(temperatures[i]).toStdString().c_str());
            file.write("\t");
            file.write(QString::number(ratios[i]).toStdString().c_str());
            file.write("\n");
        }

        file.close();
    }

    PspNsManager::PspTestFeaturesPaintCalibration feature = document->GetTestFeatures();
    feature.mPressureMaximal = maxPressure;
    feature.mPressureMinimal = minPressure;
    feature.mTemperatureMaximal = maxTemperture;
    feature.mTemperatureMinimal = minTemperature;
    document->SetTestFeatures(feature);

    QString msg;
    msg += "{\"func\":\"paintcalibration\",\"file\":\"" + fileTmpPath + "\",\"toleranceT\":" +
           QString::number(torlenceTemperature) + "}";

    /// ToDo: process data by another python process

    return true;
}
