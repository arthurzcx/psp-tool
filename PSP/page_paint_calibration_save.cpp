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
#include "page_paint_calibration_save.h"
#include "ui_page_paint_calibration_save.h"
#include "base/base_def.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QScreen>
#include <QPainter>
#include <QLayout>

using namespace PspNsBase;

PagePaintCalibrationSave::PagePaintCalibrationSave(const QString &msg, QWidget *parent,
                                                   PspNsManager::PspDocPaintCalibration *document) :
        QDialog(parent),
        ui(new Ui::PagePaintCalibrationSave),
        mJson(JsonProcess::GetInstance()->qstringToJson(msg)),
        mDocument(document) {
    ui->setupUi(this);

    QString labelText;
    for (int i = 0; i < 9; i++) {
        labelText += QString::number(i) + ": ";
        labelText += QString::number((double) mJson[(QString::number(i)).toStdString()]);

        if (i != 8) {
            labelText += "\n";
        }
    }
    ui->labelCoefficient->setText(labelText);

    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(accept()));

    ui->comboBoxPressureUnit->clear();
    ui->comboBoxPressureUnit->addItem(PressureToString(eUnitPressure::ePa));
    ui->comboBoxPressureUnit->addItem(PressureToString(eUnitPressure::eKPa));
    ui->comboBoxPressureUnit->addItem(PressureToString(eUnitPressure::eBar));
    ui->comboBoxPressureUnit->addItem(PressureToString(eUnitPressure::eMPa));
    ui->comboBoxPressureUnit->addItem(PressureToString(eUnitPressure::eGPa));
    ui->comboBoxPressureUnit->addItem(PressureToString(eUnitPressure::eAtm));
    ui->comboBoxPressureUnit->setCurrentIndex(0);

    ui->comboBoxTemperatureUnit->clear();
    ui->comboBoxTemperatureUnit->addItem(TemperatureToString(eUnitTemperature::eK));
    ui->comboBoxTemperatureUnit->addItem(TemperatureToString(eUnitTemperature::eC));
    ui->comboBoxTemperatureUnit->addItem(TemperatureToString(eUnitTemperature::eF));
    ui->comboBoxTemperatureUnit->setCurrentIndex(0);

    const PspNsManager::PspTestFeaturesPaintCalibration &feaure = mDocument->GetTestFeatures();

    ui->comboBoxPressureUnit->setCurrentIndex((int) feaure.mUnitPressure);
    ui->doubleSpinBoxPressureNormalizing->setValue(feaure.mPressureNormalizing);
    ui->doubleSpinBoxPressureMinimal->setValue(feaure.mPressureMinimal);
    ui->doubleSpinBoxPressureMaximal->setValue(feaure.mPressureMaximal);
    ui->doubleSpinBoxPressureTolerance->setValue(feaure.mPressureTolerance);

    ui->comboBoxTemperatureUnit->setCurrentIndex((int) feaure.mUnitTemperature);
    ui->doubleSpinBoxTemperatureNormalizing->setValue(feaure.mTemperatureNormalizing);
    ui->doubleSpinBoxTemperatureMinimal->setValue(feaure.mTemperatureMinimal);
    ui->doubleSpinBoxTemperatureMaximal->setValue(feaure.mTemperatureMaximal);
    ui->doubleSpinBoxTemperatureTolerance->setValue(feaure.mTemperatureTolerance);
}

PagePaintCalibrationSave::~PagePaintCalibrationSave() {
    delete ui;
}

void PagePaintCalibrationSave::reject() {
    done(QDialog::Rejected);
}

void PagePaintCalibrationSave::accept() {
    QFileDialog savePcfDialog(this, QString(tr("Save paint calibration file")));
    savePcfDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    savePcfDialog.setFileMode(QFileDialog::FileMode::AnyFile);

    QStringList fileType;
    fileType << tr("PCF file (*.pcf)");
    savePcfDialog.setNameFilters(fileType);

    if (savePcfDialog.exec() == QDialog::Accepted) {
        QString filePath = savePcfDialog.selectedFiles()[0];
        if (filePath.endsWith(".pcf"))
            filePath.replace(".pcf", "");
        filePath += ".pcf";

        Json j;
        for (int i = 0; i < 9; i++) {
            j[QString::number(i).toStdString()] = mJson[QString::number(i).toStdString()];
        }

        j["pressure_unit"] = ui->comboBoxPressureUnit->currentText().toStdString();
        j["pressure_normalizing"] = ui->doubleSpinBoxPressureNormalizing->value();
        j["pressure_minimal"] = ui->doubleSpinBoxPressureMinimal->value();
        j["pressure_maximal"] = ui->doubleSpinBoxPressureMaximal->value();
        j["pressure_tolerance"] = ui->doubleSpinBoxPressureTolerance->value();

        j["temperature_unit"] = ui->comboBoxTemperatureUnit->currentText().toStdString();
        j["temperature_normalizing"] = ui->doubleSpinBoxTemperatureNormalizing->value();
        j["temperature_minimal"] = ui->doubleSpinBoxTemperatureMinimal->value();
        j["temperature_maximal"] = ui->doubleSpinBoxTemperatureMaximal->value();
        j["temperature_tolerance"] = ui->doubleSpinBoxTemperatureTolerance->value();

        JsonProcess::GetInstance()->writeJsonFile(filePath, j);

        done(QDialog::Accepted);
    }
}
