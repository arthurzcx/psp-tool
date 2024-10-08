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
#include "dlg_paint_cali_save_param.h"
#include "ui_dlg_paint_cali_save_param.h"
#include "base/base_log.h"
#include <QDebug>
#include <QFileDialog>

using namespace PspNsBase;

DialogPaintCalibrationSaveParameter::DialogPaintCalibrationSaveParameter(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogPaintCalibrationSaveParameter) {
    ui->setupUi(this);

    Init();
}

DialogPaintCalibrationSaveParameter::~DialogPaintCalibrationSaveParameter() {
    delete ui;
}

void DialogPaintCalibrationSaveParameter::Init() {
    connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()), this, SLOT(deleteItem()));

    ui->lineEditPressure->setValidator(new QDoubleValidator(0.0, 1.0E9, 5, ui->lineEditPressure));
    ui->lineEditTempressure->setValidator(new QDoubleValidator(0.0, 1.0E9, 5, ui->lineEditTempressure));

    ui->lineEditPressure->setText("0");
    ui->lineEditTempressure->setText("0");

    QTableView *table = ui->tableViewPaintCalirbrationParameter;

    mModel = new QStandardItemModel();
    mModel->setColumnCount(2);
    table->setModel(mModel);

    table->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHeaderView *headerView = new QHeaderView(Qt::Orientation::Horizontal, table);
    QStandardItemModel *headerModel = new QStandardItemModel();
    headerModel->setRowCount(1);
    headerModel->setColumnCount(2);
    headerModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Pressure")));
    headerModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Temperature")));
    headerView->setModel(headerModel);
    table->setHorizontalHeader(headerView);
}

void DialogPaintCalibrationSaveParameter::addItem() {
    QList<QStandardItem *> list;
    list << new QStandardItem(ui->lineEditPressure->text()) << new QStandardItem(ui->lineEditTempressure->text());

    mModel->insertRow(mModel->rowCount(), list);
}

void DialogPaintCalibrationSaveParameter::deleteItem() {
    QTableView *table = ui->tableViewPaintCalirbrationParameter;
    QModelIndex index = table->currentIndex();

    mModel->removeRow(index.row());
}

void DialogPaintCalibrationSaveParameter::SaveParameter() {
    if (mModel->rowCount() < 1) {
        Log::getInstance()->writeLog(LogLevel::eWarning, tr("Nothing to be saved."));
        return;
    }

    QFileDialog savePcpDialog(this, QString(tr("Save paint calibration parameter")));
    savePcpDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    savePcpDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("PCP file (*.pcp)");
    savePcpDialog.setNameFilters(fileType);

    if (savePcpDialog.exec() == QDialog::Accepted) {
        QString filePath = savePcpDialog.selectedFiles()[0];
        if (filePath.endsWith(".pcp"))
            filePath.replace(".pcp", "");

        QFile file(filePath + ".pcp");
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            for (int index = 0; index < mModel->rowCount(); index++) {
                file.write(QString::number(index).toLocal8Bit());
                file.write(" ");

                QStandardItem *itemPressure = mModel->item(index, 0);
                QString textPressure = itemPressure->text();
                if (textPressure.isEmpty()) {
                    file.write("0");
                } else {
                    file.write(textPressure.toLocal8Bit());
                }
                file.write(" ");

                QStandardItem *itemTemperature = mModel->item(index, 1);
                QString textTemperature = itemTemperature->text();
                if (textTemperature.isEmpty()) {
                    file.write("0");
                } else {
                    file.write(textTemperature.toLocal8Bit());
                }
                file.write("\n");

            }

            file.close();
        }
    }

}
