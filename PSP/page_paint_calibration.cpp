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
#include "page_paint_calibration.h"
#include "ui_page_paint_calibration.h"
#include <QFileDialog>
#include <QQmlContext>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "algo/algo_solve.h"
#include "io/io.h"
#include "nlohmann/json.hpp"

using namespace PspNsAlgo;
using namespace PspNsImgIo;
using namespace std;
using Json = nlohmann::json;

PagePaintCalibration::PagePaintCalibration(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PagePaintCalibration) {
    ui->setupUi(this);

    mLogicPagePaintCalibration = new LogicPagePaintCalibration();
    ui->quickWidget->rootContext()->setContextProperty("logicPagePaintCalibration",
                                                       (QObject *) mLogicPagePaintCalibration);

    ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    ui->quickWidget->setClearColor(QColor(Qt::transparent));
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/qml/PagePaintCalibration.qml")));

    mQuickItem = ui->quickWidget->rootObject();
    mQuickItem->setProperty("m_width", ui->quickWidget->width());
    mQuickItem->setProperty("m_height", ui->quickWidget->height());
}

PagePaintCalibration::~PagePaintCalibration() {
    delete ui;
}


LogicPagePaintCalibration::LogicPagePaintCalibration()
        : mSeletectDirectory(""), mModelImages(""), mModelParameters("") {
#ifdef PLATFORM_WINDOWS
    mPlatformPrex = "file:///";
#else
    mPlatformPrex = "file://";
#endif
}

LogicPagePaintCalibration::~LogicPagePaintCalibration() {

}

void LogicPagePaintCalibration::reset() {
    mSeletectDirectory = "";

    mImagesPath.clear();
    mImagesName.clear();
    mRoiPoints.clear();
    mPressures.clear();
    mTemperatures.clear();

    mModelImages = "";
    mModelParameters = "";

}

bool LogicPagePaintCalibration::isValidProject() {
    bool bRet = (mImagesPath.size() != 0 &&
                 mImagesPath.size() == mImagesName.size() &&
                 mRoiPoints.size() == 2 &&
                 mPressures.size() != 0 &&
                 mPressures.size() == mTemperatures.size());

    return bRet;
}

bool LogicPagePaintCalibration::selectDirectory() {
    mSeletectDirectory = "";

    QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Select directory"));
    if (dir.isEmpty()) return false;

    mSeletectDirectory = dir;

    if (!(readImages() && readRoi() && readParameter())) {
        reset();
        return false;
    }

    if (!isValidProject()) {
        reset();
        return false;
    }

    updateModel();
    return true;
}

QString LogicPagePaintCalibration::getFileBaseName(const QString &pathName) {
    QFileInfo info(pathName);
    return info.baseName();
}

bool LogicPagePaintCalibration::readImages() {
    mImagesName.clear();
    mImagesPath.clear();

    QDir dir(mSeletectDirectory);
    dir.setFilter(QDir::Files);

    QFileInfoList fileList = dir.entryInfoList();

    for (int index = 0; index < fileList.size(); index++) {
        QFileInfo fileInfo = fileList.at(index);

        if (fileInfo.fileName().endsWith(".tif")) {
            mImagesName.push_back(fileInfo.fileName());
            mImagesPath.push_back(mSeletectDirectory + "/" + fileInfo.fileName());
        }
    }

    bool bRet = (mImagesName.size() == mImagesPath.size() && !mImagesName.empty() && !mImagesPath.empty());
    if (!bRet) {
        QMessageBox::warning(0, tr("Warning"), tr("No images in this directory."));
    }
    return bRet;
}

bool LogicPagePaintCalibration::readRoi() {
    mRoiPoints.clear();

    QString roiFileName;

    QDir dir(mSeletectDirectory);
    dir.setFilter(QDir::Files);

    QFileInfoList fileList = dir.entryInfoList();

    for (int index = 0; index < fileList.size(); index++) {
        QFileInfo fileInfo = fileList.at(index);

        if (fileInfo.fileName().endsWith(".pcr")) {
            roiFileName = fileInfo.fileName();

            break;
        }
    }

    if (roiFileName.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: ROI file is empty!"));
        msgBox.exec();
        return false;

    }

    QFile file(mSeletectDirectory + "/" + roiFileName);
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        QString stringRoi = QString::fromStdString(file.readAll().toStdString());

        QStringList linesList = stringRoi.split("\n");
        if (!linesList.at(0).isEmpty()) {
            QStringList line = linesList.at(0).split(" ");
            if (line.size() != 4) {
                file.close();
                return false;
            }

            QPointF start(line.at(0).toDouble(), line.at(1).toDouble());
            QPointF end(line.at(2).toDouble(), line.at(3).toDouble());

            mRoiPoints.push_back(start);
            mRoiPoints.push_back(end);
        }

        file.close();
    }

    bool bRet = (mRoiPoints.size() == 2);
    if (!bRet) {
        QMessageBox::warning(0, tr("Warning"), tr("No ROI file(*.pcr) in this directory."));
    }

    return bRet;
}

bool LogicPagePaintCalibration::readParameter() {
    mPressures.clear();
    mTemperatures.clear();

    QString parameterFileName;

    QDir dir(mSeletectDirectory);
    dir.setFilter(QDir::Files);

    QFileInfoList fileList = dir.entryInfoList();

    for (int index = 0; index < fileList.size(); index++) {
        QFileInfo fileInfo = fileList.at(index);
        if (fileInfo.fileName().endsWith(".pcp")) {
            parameterFileName = fileInfo.fileName();
            break;
        }
    }

    if (parameterFileName.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: Parameter file is empty!"));
        msgBox.exec();
        return false;
    }

    QFile file(mSeletectDirectory + "/" + parameterFileName);
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        QString stringParameter = QString::fromStdString(file.readAll().toStdString());

        QStringList linesList = stringParameter.split("\n");
        for (int index = 0; index < linesList.size(); index++) {
            QStringList line = linesList.at(index).split(" ");
            if (line.size() != 3) continue;

            mPressures.push_back(line.at(1));
            mTemperatures.push_back(line.at(2));
        }

        file.close();
    }

    bool bRet = (mPressures.size() != 0 && mPressures.size() == mTemperatures.size());
    if (!bRet) {
        QMessageBox::warning(0, tr("Warning"), tr("No parameter file(*.pcp) in this directory."));
    }

    return bRet;
}

void LogicPagePaintCalibration::updateModel() {
    Json j;
    for (size_t index = 0; index < mImagesName.size(); index++) {
        Json jImage;
        jImage["url"] = (mPlatformPrex + mImagesPath[index]).toStdString();
        jImage["name"] = mImagesName[index].toStdString();
        jImage["P"] = "";
        jImage["T"] = "";
        jImage["isBackground"] = false;
        jImage["isReference"] = false;

        j.push_back(jImage);
    }
    Json jModel;
    jModel["model"] = j;

    mModelImages = QString::fromStdString(jModel.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));

    Json jParameter;
    for (size_t index = 0; index < mPressures.size(); index++) {
        Json jTmp;
        jTmp["P"] = mPressures[index].toStdString();
        jTmp["T"] = mTemperatures[index].toStdString();

        jParameter.push_back(jTmp);
    }

    Json jModelParameter;
    jModelParameter["model"] = jParameter;
    mModelParameters = QString::fromStdString(
            jModelParameter.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore));;
}

QString LogicPagePaintCalibration::getModelImage() {
    return mModelImages;
}

QString LogicPagePaintCalibration::getModelParameter() {
    return mModelParameters;
}

void LogicPagePaintCalibration::setModelImage(const QString &modelStr) {
    mModelImages = modelStr;

    fit();
}

void LogicPagePaintCalibration::fit() {
    cv::Mat backgroundImg, referenceImg;
    vector<cv::Mat> generalImags;
    vector<double> pressures, temperatures;

    float top = min(mRoiPoints[0].y(), mRoiPoints[1].y());
    float left = min(mRoiPoints[0].x(), mRoiPoints[1].x());
    float bottom = max(mRoiPoints[0].y(), mRoiPoints[1].y());
    float right = max(mRoiPoints[0].x(), mRoiPoints[1].x());
    cv::Rect2f rt(left, top, right - left, bottom - top);

    Json objJson = Json::parse(mModelImages.toUtf8());
    Json arrayJson = objJson["model"];
    for (auto iter = arrayJson.begin(); iter != arrayJson.end(); ++iter) {
        Json obj = *iter;

        if (obj["isBackground"] == true) {
            Img_io io;
            io.readImg(QString::fromStdString(obj["url"]).replace(mPlatformPrex, "").toLocal8Bit().data());
            backgroundImg = io.getSrcImg()(rt);

        } else if (obj["isReference"] == true) {
            Img_io io;
            io.readImg(QString::fromStdString(obj["url"]).replace(mPlatformPrex, "").toLocal8Bit().data());
            referenceImg = io.getSrcImg()(rt);
            generalImags.push_back(referenceImg);

            if (!QString::fromStdString(obj["P"]).isEmpty() && !QString::fromStdString(obj["T"]).isEmpty()) {
                pressures.push_back(QString::fromStdString(obj["P"]).toDouble());
                temperatures.push_back(QString::fromStdString(obj["T"]).toDouble());
            }
        } else {
            Img_io io;
            io.readImg(QString::fromStdString(obj["url"]).replace(mPlatformPrex, "").toLocal8Bit().data());
            generalImags.push_back(io.getSrcImg()(rt));

            if (!QString::fromStdString(obj["P"]).isEmpty() && !QString::fromStdString(obj["T"]).isEmpty()) {
                pressures.push_back(QString::fromStdString(obj["P"]).toDouble());
                temperatures.push_back(QString::fromStdString(obj["T"]).toDouble());
            }
        }
    }

    if (generalImags.size() != pressures.size() || generalImags.size() != temperatures.size()) {
        assert(false);
    }

    cv::Mat referenceMinusImg = referenceImg - backgroundImg;
    vector<double> ratios;
    for (size_t i = 0; i < generalImags.size(); i++) {
        cv::Mat generalMinuxImg = generalImags[i] - backgroundImg;
        cv::Mat dst;
        ratios.push_back(calAverage(referenceMinusImg, generalMinuxImg));
    }

    QString appPath = QCoreApplication::applicationDirPath();
    QString fileTmpPath = appPath + "/paintcalibrationTmp";

    QFile file(fileTmpPath);
    if (file.exists()) {
        file.remove();
    }
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        for (size_t i = 0; i < ratios.size(); i++) {
            file.write(QString::number(pressures[i]).toStdString().c_str());
            file.write("\t");
            file.write(QString::number(temperatures[i]).toStdString().c_str());
            file.write("\t");
            file.write(QString::number(ratios[i]).toStdString().c_str());
            file.write("\n");
        }

        file.close();
    }

    QString msg;
    msg += "{\"func\":\"paintcalibration\",\"file\":\"" + fileTmpPath + "\"}";

    /// ToDo: process data by another python process here
}
