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

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QFileDialog>

namespace PspNsOp {
    class Operator;
    class OperatorFactory;
    class OperatorManager;
}
using namespace PspNsOp;

class PspOperatorUtil : public QObject {
Q_OBJECT

public:
    PspOperatorUtil();

    void TranslateUi();

private:
    void InitMenu();

    void InitMenuMarkerAdd();
    void InitMenuMarkerDelete();
    void InitMenuPaintCalibrationSaveRange();

public slots:
    void PopMarkerAdd();
    void SetMarkerAddBoxSize();

    void PopPaintCalibrationSave();
    void PaintCalibrationSave();

private:

    QMenu *mMenuMarkerAdd;
    QAction *mActionMarkerAdd_SetSize;
    QAction *mActionMarkerAdd_Exit;

    QMenu *mMenuMarkerDelete;

    QMenu *mMenuPaintCalibrationSaveRange;
    QAction *mActionPaintCalibrationSave;
    QAction *mActionPaintCalibrationSaveExit;
};
