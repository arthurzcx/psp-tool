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
#include "psp_operator_util.h"
#include "op/op_base.h"
#include "op/op_factory.h"
#include "op/op_manager.h"
#include "op/op_markeradd.h"
#include "op/op_markerdelete.h"
#include "op/op_paint_calibration_range.h"
#include "dlg_op_marker_add_size.h"
#include "psp_view_manager.h"

PspOperatorUtil::PspOperatorUtil()
        : mMenuMarkerAdd(nullptr), mMenuMarkerDelete(nullptr), mActionMarkerAdd_SetSize(nullptr),
          mActionMarkerAdd_Exit(nullptr) {
    InitMenu();
    TranslateUi();
}

void PspOperatorUtil::TranslateUi() {
    if (mActionMarkerAdd_SetSize) mActionMarkerAdd_SetSize->setText(tr("Set box size"));
    if (mActionMarkerAdd_Exit) mActionMarkerAdd_Exit->setText(tr("Exit"));

    if (mActionPaintCalibrationSave) mActionPaintCalibrationSave->setText(tr("Save range"));
    if (mActionPaintCalibrationSaveExit) mActionPaintCalibrationSaveExit->setText(tr("Exit"));
}

void PspOperatorUtil::InitMenu() {
    InitMenuMarkerAdd();
    InitMenuMarkerDelete();
    InitMenuPaintCalibrationSaveRange();
}

void PspOperatorUtil::InitMenuMarkerAdd() {
    mMenuMarkerAdd = new QMenu();

    mActionMarkerAdd_SetSize = new QAction(0);
    mActionMarkerAdd_Exit = new QAction(0);

    mMenuMarkerAdd->addAction(mActionMarkerAdd_SetSize);
    mMenuMarkerAdd->addAction(mActionMarkerAdd_Exit);

    OperatorMarkerAdd *pOperatorAdd = dynamic_cast<OperatorMarkerAdd *>(OperatorFactory::CreateOperator(
            OperatorType::eOperatorMarkerAdd));
    connect(pOperatorAdd, SIGNAL(PopMenu()), this, SLOT(PopMarkerAdd()));
    connect(mActionMarkerAdd_SetSize, SIGNAL(triggered(bool)), this, SLOT(SetMarkerAddBoxSize()));
    connect(mActionMarkerAdd_Exit, SIGNAL(triggered(bool)), pOperatorAdd, SLOT(ExitOperator()));
}

void PspOperatorUtil::InitMenuMarkerDelete() {
    mMenuMarkerDelete = new QMenu();
}

void PspOperatorUtil::InitMenuPaintCalibrationSaveRange() {
    mMenuPaintCalibrationSaveRange = new QMenu();

    mActionPaintCalibrationSave = new QAction(0);
    mActionPaintCalibrationSaveExit = new QAction(0);

    mMenuPaintCalibrationSaveRange->addAction(mActionPaintCalibrationSave);
    mMenuPaintCalibrationSaveRange->addAction(mActionPaintCalibrationSaveExit);

    OperatorPaintCalibrationRange *pOperatorPaintCalibrationSaveRange = dynamic_cast<OperatorPaintCalibrationRange *>(OperatorFactory::CreateOperator(
            OperatorType::eOperatorPaintCalibrationRange));
    connect(pOperatorPaintCalibrationSaveRange, SIGNAL(PopMenu()), this, SLOT(PopPaintCalibrationSave()));
    connect(mActionPaintCalibrationSave, SIGNAL(triggered(bool)), this, SLOT(PaintCalibrationSave()));
    connect(mActionPaintCalibrationSaveExit, SIGNAL(triggered(bool)), pOperatorPaintCalibrationSaveRange,
            SLOT(ExitOperator()));
}

void PspOperatorUtil::PopMarkerAdd() {
    mMenuMarkerAdd->exec(QCursor::pos());
}

void PspOperatorUtil::PopPaintCalibrationSave() {
    mMenuPaintCalibrationSaveRange->exec(QCursor::pos());
}

void PspOperatorUtil::SetMarkerAddBoxSize() {
    OperatorMarkerAdd *pOperatorAdd = dynamic_cast<OperatorMarkerAdd *>(OperatorFactory::CreateOperator(
            OperatorType::eOperatorMarkerAdd));

    Dialog_OperatorMarkerAdd_Size dlgSetMarkerAddSize;
    dlgSetMarkerAddSize.SetBoxSize(pOperatorAdd->GetBoxSize());

    if (dlgSetMarkerAddSize.exec() == QDialog::Accepted) {
        pOperatorAdd->SetBoxSize(dlgSetMarkerAddSize.GetBoxSize());

        PspViewManager::GetInstance()->UpdateView();
    }
}

void PspOperatorUtil::PaintCalibrationSave() {
    QFileDialog savePaintCalibrationRangeDialog(0, tr("Save paint calibration range"));

    savePaintCalibrationRangeDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    savePaintCalibrationRangeDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("PCR file (*.pcr)");
    savePaintCalibrationRangeDialog.setNameFilters(fileType);

    if (savePaintCalibrationRangeDialog.exec() == QDialog::Accepted) {
        QString filePath = savePaintCalibrationRangeDialog.selectedFiles()[0];
        if (filePath.endsWith(".pcr"))
            filePath.replace(".pcr", "");

        QFile file(filePath + ".pcr");
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            OperatorPaintCalibrationRange *pOperatorPaintCalibrationSaveRange = dynamic_cast<OperatorPaintCalibrationRange *>(OperatorFactory::CreateOperator(
                    OperatorType::eOperatorPaintCalibrationRange));

            const QPointF &start = pOperatorPaintCalibrationSaveRange->GetStartPoint();
            const QPointF &end = pOperatorPaintCalibrationSaveRange->GetEndPoint();

            file.write(QString::number(start.x()).toLocal8Bit());
            file.write(" ");
            file.write(QString::number(start.y()).toLocal8Bit());
            file.write(" ");
            file.write(QString::number(end.x()).toLocal8Bit());
            file.write(" ");
            file.write(QString::number(end.y()).toLocal8Bit());

            file.close();

            pOperatorPaintCalibrationSaveRange->ExitOperator();
        }
    }
}
