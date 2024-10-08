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
#include "page_pseudo_config.h"
#include "ui_page_pseudo_config.h"
#include "manager/work_manager.h"
#include "base/base_graphic_colorbar.h"
#include "view/view_disp.h"
#include <QThread>

PagePseudoConfig::PagePseudoConfig(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PagePseudoConfig),
        mMinVal(0),
        mMaxVal(0),
        mSliderRange(200),
        mMinRealVal(0.0),
        mMaxRealVal(0.0),
        mStep(0.0) {
    ui->setupUi(this);

    Init();

    connect(ui->minValueSlider, SIGNAL(sliderReleased()), this, SLOT(slotMinSliderValueChanged()));
    connect(ui->maxValueSlider, SIGNAL(sliderReleased()), this, SLOT(slotMaxSliderValueChanged()));

    connect(ui->doubleSpinBoxSliderMin, SIGNAL(valueChanged(double)), this, SLOT(slotMinSpinValueChanged(double)));
    connect(ui->doubleSpinBoxSliderMax, SIGNAL(valueChanged(double)), this, SLOT(slotMaxSpinValueChanged(double)));

    if (GraphicColorBar::GetInstance()) {
        ui->doubleSpinBoxSliderMin->setValue(GraphicColorBar::GetInstance()->GetColorMin());
        ui->doubleSpinBoxSliderMax->setValue(GraphicColorBar::GetInstance()->GetColorMax());
    }
}

PagePseudoConfig::~PagePseudoConfig() {
    delete ui;
}

void PagePseudoConfig::Init() {
    cv::Mat curImg = PspNsManager::WorkImgManager::GetInstance()->getCurImg();
    if (curImg.empty()) {
        assert(false);
        return;
    }


    double minVal, maxVal;
    cv::minMaxLoc(curImg, &minVal, &maxVal);

    if (maxVal - minVal < DBL_EPSILON) {
        assert(false);
        return;
    }

    mMinRealVal = minVal;
    mMaxRealVal = maxVal;

    mStep = 0.01;
    mSliderRange = (mMaxRealVal - mMinRealVal) / mStep;
    if (mSliderRange >= 5000) {
        mSliderRange = 5000;
        mStep = (mMaxRealVal - mMinRealVal) / mSliderRange;
    }

    ui->labelRangeValue->setText(
            tr("Min: ") + QString::number(minVal, 'g', 5) + tr(" Max: ") + QString::number(maxVal, 'g', 5));

    ui->doubleSpinBoxSliderMin->setMinimum(minVal);
    ui->doubleSpinBoxSliderMin->setMaximum(maxVal);
    ui->doubleSpinBoxSliderMin->setSingleStep(mStep);

    ui->doubleSpinBoxSliderMax->setMinimum(minVal);
    ui->doubleSpinBoxSliderMax->setMaximum(maxVal);
    ui->doubleSpinBoxSliderMax->setSingleStep(mStep);

    mMinVal = 0;
    mMaxVal = mSliderRange;

    ui->minValueSlider->setRange(0, mSliderRange);
    ui->maxValueSlider->setRange(0, mSliderRange);
    ui->minValueSlider->setSingleStep(1);
    ui->maxValueSlider->setSingleStep(1);
    ui->minValueSlider->setValue(mMinVal);
    ui->maxValueSlider->setValue(mMaxVal);
}

void PagePseudoConfig::slotMinSpinValueChanged(double val) {
    if (val <= mMinRealVal || val >= mMaxRealVal) return;

    ui->minValueSlider->setValue((val - mMinRealVal) / mStep);
}

void PagePseudoConfig::slotMaxSpinValueChanged(double val) {
    if (val <= mMinRealVal || val >= mMaxRealVal) return;

    ui->maxValueSlider->setValue((val - mMinRealVal) / mStep);
}

void PagePseudoConfig::slotMinSliderValueChanged() {
    int value = ui->minValueSlider->value();

    ui->doubleSpinBoxSliderMin->setValue(mMinRealVal + value * mStep);
}

void PagePseudoConfig::slotMaxSliderValueChanged() {
    int value = ui->maxValueSlider->value();

    ui->doubleSpinBoxSliderMax->setValue(mMinRealVal + value * mStep);
}

void PagePseudoConfig::accept() {
    QDialog::accept();

    double a = ui->minValueSlider->value() * mStep + mMinRealVal;
    double b = ui->maxValueSlider->value() * mStep + mMinRealVal;
    double minVal = min(a, b);
    double maxVal = max(a, b);
    if (fabs(maxVal - minVal) < FLT_EPSILON) {
        return;
    }

    GraphicColorBar::GetInstance()->SetColorMin(minVal);
    GraphicColorBar::GetInstance()->SetColorMax(maxVal);

    cv::Mat curImg = PspNsManager::WorkImgManager::GetInstance()->getCurImg();
    cv::Mat tmp(curImg.rows, curImg.cols, CV_8UC1);
    for (int i = 0; i < curImg.rows; i++) {
        for (int j = 0; j < curImg.cols; j++) {
            if (curImg.depth() == CV_16U) {
                if (curImg.at<ushort>(i, j) < minVal) {
                    tmp.at<uchar>(i, j) = 0;
                } else if (curImg.at<ushort>(i, j) > maxVal) {
                    tmp.at<uchar>(i, j) = 255;
                } else {
                    tmp.at<uchar>(i, j) = (curImg.at<ushort>(i, j) - minVal) * 255.0 / (maxVal - minVal);
                }
            } else if (curImg.depth() == CV_32F) {
                if (curImg.at<float>(i, j) < minVal) {
                    tmp.at<uchar>(i, j) = 0;
                } else if (curImg.at<float>(i, j) > maxVal) {
                    tmp.at<uchar>(i, j) = 255;
                } else {
                    tmp.at<uchar>(i, j) = (curImg.at<float>(i, j) - minVal) * 255.0 / (maxVal - minVal);
                }
            } else {
                assert(false);
            }
        }
    }

    cv::cvtColor(tmp, tmp, cv::COLOR_GRAY2RGB);

    mResultImage = QImage((unsigned char *) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888).copy();
}

const QImage &PagePseudoConfig::GetResultImage() const {
    return mResultImage;
}
