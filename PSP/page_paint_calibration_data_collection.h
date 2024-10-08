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
#pragma once

#include <QDialog>
#include "manager/manager_doc_paint_calibration.h"

namespace Ui {
    class PagePaintCalibrationDataCollection;
}

class PagePaintCalibrationDataCollection : public QDialog {
Q_OBJECT

public:
    explicit PagePaintCalibrationDataCollection(PspNsManager::PspDocPaintCalibration *document,
                                                QWidget *parent = nullptr);
    ~PagePaintCalibrationDataCollection();

public slots:
    void dataFromCamera();
    void dataFromDisk();

private:
    Ui::PagePaintCalibrationDataCollection *ui;

    PspNsManager::PspDocPaintCalibration *mDocument;
};
