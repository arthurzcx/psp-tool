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
#include "page_paint_calibration_data_collection.h"
#include "ui_page_paint_calibration_data_collection.h"
#include "psp_paint_calibration_data_from_disk.h"

PagePaintCalibrationDataCollection::PagePaintCalibrationDataCollection(PspNsManager::PspDocPaintCalibration *document,
                                                                       QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PagePaintCalibrationDataCollection),
        mDocument(document) {
    ui->setupUi(this);

    connect(ui->pushButtonCamera, SIGNAL(pressed()), this, SLOT(dataFromCamera()));
    connect(ui->pushButtonComputer, SIGNAL(pressed()), this, SLOT(dataFromDisk()));
}

PagePaintCalibrationDataCollection::~PagePaintCalibrationDataCollection() {
    delete ui;
}

void PagePaintCalibrationDataCollection::dataFromCamera() {
    QDialog::accept();
}

void PagePaintCalibrationDataCollection::dataFromDisk() {
    if (mDocument) {
        PaintCalibrationDataFromDisk::GetInstance()->Init(mDocument);
    }

    QDialog::accept();
}
