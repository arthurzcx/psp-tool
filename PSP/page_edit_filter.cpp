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
#include "page_edit_filter.h"
#include "ui_page_edit_filter.h"
#include <qdebug.h>
#include <QMessageBox>
#include "algo/algo_filter.h"
#include "manager/work_manager.h"

using namespace PspNsAlgo;
using namespace PspNsManager;

PspEditFilter::PspEditFilter(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PspEditFilter) {
    ui->setupUi(this);

    translateUI();
}

PspEditFilter::~PspEditFilter() {
    delete ui;
}

void PspEditFilter::translateUI() {
    ui->comboBoxFilterType->clear();
    ui->comboBoxFilterType->addItem(tr("Box filter"));
    ui->comboBoxFilterType->addItem(tr("Mean filter"));
    ui->comboBoxFilterType->addItem(tr("Median filter"));
    ui->comboBoxFilterType->addItem(tr("Gauss filter"));

    ui->labelFilterType->setText(tr("Filter type"));
    ui->labelFilterType->setText(tr("Filter width"));
    ui->labelFilterIteration->setText(tr("Filter iteration"));

    ui->pushButtonCaccel->setText(tr("Cancel"));
    ui->pushButtonOk->setText(tr("Ok"));
}

void PspEditFilter::reject() {
    done(QDialog::Rejected);
}

void PspEditFilter::accept() {
    int filterType = ui->comboBoxFilterType->currentIndex();
    int filterWidth = ui->spinBox->value();

    int iteration = ui->spinBoxIteration->value();
    if (filterWidth > 20) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Warning: The filterWidth should be between 3~20."));
        msgBox.exec();
        return;
    }
    filterWidth = fmin(fmax(filterWidth, 3), 20);

    cv::Mat curImg = WorkImgManager::GetInstance()->getCurImg();
    cv::Mat dstImg;
    curImg.copyTo(dstImg);

    switch (filterType) {

        case (int) FilterType::FILTER_BOX:
            for (int i = 0; i < iteration; i++) {
                filterBox(dstImg, dstImg, -1, cv::Size(filterWidth, filterWidth));
            }
            WorkImgManager::GetInstance()->setCurImg(dstImg);
            break;

        case (int) FilterType::FILTER_MEAN:
            for (int i = 0; i < iteration; i++) {
                filterMean(dstImg, dstImg, cv::Size(filterWidth, filterWidth));
            }
            WorkImgManager::GetInstance()->setCurImg(dstImg);
            break;

        case (int) FilterType::FILTER_MEDIAN:
            if (filterWidth % 2 == 0) {
                filterWidth = filterWidth - 1;
            }
            if (filterWidth > 5) {
                QMessageBox msgBox;
                msgBox.setText(QObject::tr("Warning: The filterWidth shouldn't be more than 5"));
                msgBox.exec();
                return;
            }
            for (int i = 0; i < iteration; i++) {
                filterMedian(dstImg, dstImg, filterWidth);
            }
            WorkImgManager::GetInstance()->setCurImg(dstImg);
            break;

        case (int) FilterType::FILTER_GAUSSIAN:
            if (filterWidth % 2 == 0) {
                filterWidth = filterWidth - 1;
            }
            for (int i = 0; i < iteration; i++) {
                filterGaussian(dstImg, dstImg, cv::Size(filterWidth, filterWidth), 0);
            }
            WorkImgManager::GetInstance()->setCurImg(dstImg);
            break;

        case (int) FilterType::FILTER_BILATERAL:
            filterBilateral(curImg, dstImg, filterWidth, 255, 0);
            WorkImgManager::GetInstance()->setCurImg(dstImg);
            break;

        default:
            break;
    }

    done(QDialog::Accepted);
}
