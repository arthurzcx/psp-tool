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
#include "page_two_image_calculation.h"
#include "ui_page_two_image_calculation.h"
#include "io/io.h"
#include "algo/algo_basic_arithmatic.h"
#include "manager/work_manager.h"
#include <QFileDialog>
#include <QQmlContext>
#include <QMessageBox>

PageTwoImageCalculation::PageTwoImageCalculation(QWidget *parent, eTypeTwoImageCalculation type) :
        QDialog(parent),
        ui(new Ui::PageTwoImageCalculation),
        mTypeCalculation(type) {
    ui->setupUi(this);

    switch ((int) type) {
        case eTypeTwoImageCalculation::eAddition: {
            setWindowTitle(tr("Image addition"));
            break;
        }
        case eTypeTwoImageCalculation::eSubtract: {
            setWindowTitle(tr("Image subtraction"));
            break;
        }
        case eTypeTwoImageCalculation::eDivision: {
            setWindowTitle(tr("Image division"));
            break;
        }
        default: {
            break;
        }
    }

    logicTwoImageCalculation = new LogicTwoImageCalculation();
    ui->quickWidget->rootContext()->setContextProperty("logicTwoImageCalculation",
                                                       (QObject *) logicTwoImageCalculation);

    ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    ui->quickWidget->setClearColor(QColor(Qt::transparent));
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/qml/PageTwoImageCalculation.qml")));

    mQuickItem = ui->quickWidget->rootObject();
    mQuickItem->setProperty("m_width", ui->quickWidget->width());
    mQuickItem->setProperty("m_height", ui->quickWidget->height());

    connect(logicTwoImageCalculation, SIGNAL(reject()), this, SLOT(reject()));
    connect(logicTwoImageCalculation, SIGNAL(accept()), this, SLOT(accept()));
}

PageTwoImageCalculation::~PageTwoImageCalculation() {
    delete ui;
}

void PageTwoImageCalculation::accept() {
    logicTwoImageCalculation->calculation(mTypeCalculation);

    QDialog::accept();
}

LogicTwoImageCalculation::LogicTwoImageCalculation() {
    mFirstImagePath = "";
    mSecondImagePath = "";

#ifdef PLATFORM_WINDOWS
    mPlatformPrex = "file:///";
#else
    mPlatformPrex = "file://";
#endif
}

LogicTwoImageCalculation::~LogicTwoImageCalculation() {

}

void LogicTwoImageCalculation::emitReject() {
    emit reject();
}

void LogicTwoImageCalculation::emitAccept() {
    emit accept();
}

void LogicTwoImageCalculation::selectFirstImage() {
    if (selectImage(true)) {
        emit updateFirstImage(mPlatformPrex + mFirstImagePath);
    }
}

void LogicTwoImageCalculation::selectSecondImage() {
    if (selectImage(false)) {
        emit updateSecondImage(mPlatformPrex + mSecondImagePath);
    }
}

bool LogicTwoImageCalculation::selectImage(bool isFirst) {
    QString imageFilePath = "", markerFilePath = "";

    QFileDialog fileDialog(0, isFirst ? QString(tr("Open the first image")) : QString(tr("Open the second image")));
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

        if (isFirst) {
            mFirstImagePath = imageFilePath;
        } else {
            mSecondImagePath = imageFilePath;
        }

        return true;
    }

    return false;
}

void LogicTwoImageCalculation::calculation(eTypeTwoImageCalculation type) {
    if (mFirstImagePath.isEmpty() || mSecondImagePath.isEmpty()) return;

    PspNsImgIo::Img_io io;
    cv::Mat firstImg, secondImg;
    if (io.readImg(mFirstImagePath.toStdString())) {
        firstImg = io.getSrcImg();
    }
    if (io.readImg(mSecondImagePath.toStdString())) {
        secondImg = io.getSrcImg();
    }

    if (firstImg.rows != secondImg.rows || firstImg.cols != secondImg.cols || firstImg.depth() != secondImg.depth()) {
        return;
    }

    cv::Mat dst;
    switch (type) {
        case eTypeTwoImageCalculation::eAddition: {
            if (firstImg.cols != secondImg.cols ||
                firstImg.rows != secondImg.rows ||
                firstImg.type() != secondImg.type()) {
                QMessageBox msgBox;
                msgBox.setText(tr("Two images has different size or type!"));
                return;
            }

            PspNsAlgo::add(firstImg, secondImg, dst);
            break;
        }
        case eTypeTwoImageCalculation::eSubtract: {
            if (firstImg.cols != secondImg.cols ||
                firstImg.rows != secondImg.rows ||
                firstImg.type() != secondImg.type()) {
                QMessageBox msgBox;
                msgBox.setText(tr("Two images has different size or type!"));
                return;
            }
            PspNsAlgo::subtract(firstImg, secondImg, dst);
            break;
        }
        case eTypeTwoImageCalculation::eDivision: {
            if (firstImg.cols != secondImg.cols ||
                firstImg.rows != secondImg.rows ||
                firstImg.type() != secondImg.type()) {
                QMessageBox msgBox;
                msgBox.setText(tr("Two images has different size or type!"));
                return;
            }
            cv::Mat firstF32, secondF32;
            firstImg.convertTo(firstF32, CV_32F);
            secondImg.convertTo(secondF32, CV_32F);
            PspNsAlgo::divide(firstF32, secondF32, dst);
            break;
        }
        default: {
            break;
        }
    }

    PspNsManager::WorkImgManager::GetInstance()->setCurImg(dst);
}
