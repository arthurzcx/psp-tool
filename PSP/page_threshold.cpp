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
#include "page_threshold.h"
#include "ui_page_threshold.h"
#include "algo/algo_threshold.h"
#include "manager/work_manager.h"

using namespace PspNsAlgo;
using namespace PspNsManager;

PageThreshold::PageThreshold(cv::Mat inputMat, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PageThreshold) {
    ui->setupUi(this);

    inputMat.copyTo(mMat);

    Init();
}

void PageThreshold::Init() {
    double thresholdLevel = 100.0;

    cv::minMaxLoc(mMat, &mMinVal, &mMaxVal);

    ui->spinBoxThreshold->setMinimum(mMinVal);
    ui->spinBoxThreshold->setMaximum(mMaxVal);
    mStepSpinBox = (mMaxVal - mMinVal) / thresholdLevel;
    ui->spinBoxThreshold->setSingleStep(mStepSpinBox);

    ui->sliderThreshold->setMinimum(0);
    ui->sliderThreshold->setMaximum(thresholdLevel);
    mStepSlider = 1;
    ui->sliderThreshold->setSingleStep(mStepSpinBox);

    connect(ui->spinBoxThreshold, SIGNAL(valueChanged(double)), this, SLOT(SetSliderVal(double)));
    connect(ui->sliderThreshold, SIGNAL(valueChanged(int)), this, SLOT(SetSpinVal(int)));

    double highThreshold;
    calcAdapiveThreshold(mMat, mCurrentThreshold, highThreshold);

    ui->spinBoxThreshold->setValue(mCurrentThreshold);

    UpdateResult();
}

void PageThreshold::SetSpinVal(int val) {
    mCurrentThreshold = mMinVal + mStepSpinBox * val;

    ui->spinBoxThreshold->setValue(mCurrentThreshold);

    if (fabs(mCurrentThreshold - mMinVal) > FLT_EPSILON) {
        UpdateResult();
    }
}

void PageThreshold::SetSliderVal(double val) {
    ui->sliderThreshold->setValue((val - mMinVal) / mStepSpinBox);
}

PageThreshold::~PageThreshold() {
    delete ui;
}

void PageThreshold::UpdateResult() {
    cv::Mat tmp;
    mMat.copyTo(tmp);

    switch (tmp.depth()) {
        case CV_8U: {
            for (int i = 0; i < tmp.rows; i++) {
                for (int j = 0; j < tmp.cols; j++) {
                    if (tmp.at<uchar>(i, j) < mCurrentThreshold) {
                        tmp.at<uchar>(i, j) = 0;
                    } else {
                        tmp.at<uchar>(i, j) = 255;
                    }
                }
            }
            break;
        }
        case CV_16U: {
            for (int i = 0; i < tmp.rows; i++) {
                for (int j = 0; j < tmp.cols; j++) {
                    if (tmp.at<ushort>(i, j) < mCurrentThreshold) {
                        tmp.at<ushort>(i, j) = 0;
                    } else {
                        tmp.at<ushort>(i, j) = 255;
                    }
                }
            }
            break;
        }
        case CV_32F: {
            for (int i = 0; i < tmp.rows; i++) {
                for (int j = 0; j < tmp.cols; j++) {
                    if (tmp.at<float>(i, j) < mCurrentThreshold) {
                        tmp.at<float>(i, j) = 0;
                    } else {
                        tmp.at<float>(i, j) = 255;
                    }
                }
            }
            break;
        }
        default: {
            assert(false);
            break;
        }
    }

    WorkImgManager::GetInstance()->setCurImg(tmp);
}

void PageThreshold::on_buttonBox_accepted() {
    PspNsAlgo::thresholdSegmentation(mMat, mCurrentThreshold);

    WorkImgManager::GetInstance()->setCurImg(mMat);

    QDialog::accepted();
}

void PageThreshold::on_buttonBox_rejected() {
    WorkImgManager::GetInstance()->setCurImg(mMat);

    QDialog::reject();
}
