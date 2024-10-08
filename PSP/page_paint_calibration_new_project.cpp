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
#include "page_paint_calibration_new_project.h"
#include "ui_page_paint_calibration_new_project.h"

PagePaintCalibrationNewProject::PagePaintCalibrationNewProject(PspNsManager::PspDocPaintCalibration *document,
                                                               QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PagePaintCalibrationNewProject),
        mDocument(document) {
    ui->setupUi(this);

    translateUi();

    initUi();
}

PagePaintCalibrationNewProject::~PagePaintCalibrationNewProject() {
    delete ui;
}

void PagePaintCalibrationNewProject::translateUi() {
    setWindowTitle(tr("Paint calibration new project"));

    ui->groupBoxFeatures->setTitle(tr("Features"));
    ui->labelTestName->setText(tr("Test id"));
    ui->labelModelName->setText(tr("Model id"));
    ui->labelPointName->setText(tr("Point id"));
    ui->labelPaintName->setText(tr("Paint name"));
    ui->labelFormulaType->setText(tr("Calibration power"));
    ui->labelTestDate->setText(tr("Date"));
    ui->labelTestDesc->setText(tr("Description"));

    ui->groupBoxPressure->setTitle(tr("Pressure"));
    ui->labelPressureMaximal->setText(tr("Maximal"));
    ui->labelPressureMinimal->setText(tr("Mimimal"));
    ui->labelPressureNormalizing->setText(tr("Normalizing"));
    ui->labelPressureTolerance->setText(tr("Tolerance"));
    ui->labelPressureUnit->setText(tr("Unit"));

    ui->groupBoxTemperature->setTitle(tr("Temperature"));
    ui->labelTemperatureMaximal->setText(tr("Maximal"));
    ui->labelTemperatureMinimal->setText(tr("Mimimal"));
    ui->labelTemperatureNormalizing->setText(tr("Normalizing"));
    ui->labelTemperatureTolerance->setText(tr("Tolerance"));
    ui->labelTemperatureUnit->setText(tr("Unit"));

    ui->comboBoxFormulaType->clear();
    ui->comboBoxFormulaType->addItem(tr("Quadratic"));
    ui->comboBoxFormulaType->setCurrentIndex(0);

    ui->comboBoxPressureUnit->clear();
    ui->comboBoxPressureUnit->addItem(tr("Pa"));
    ui->comboBoxPressureUnit->addItem(tr("KPa"));
    ui->comboBoxPressureUnit->addItem(tr("MPa"));
    ui->comboBoxPressureUnit->addItem(tr("Bar"));
    ui->comboBoxPressureUnit->addItem(tr("MPa"));
    ui->comboBoxPressureUnit->addItem(tr("GPa"));
    ui->comboBoxPressureUnit->addItem(tr("Atm"));
    ui->comboBoxPressureUnit->setCurrentIndex(0);

    ui->comboBoxTemperatureUnit->clear();
    ui->comboBoxTemperatureUnit->addItem(tr("K"));
    ui->comboBoxTemperatureUnit->addItem(tr("C"));
    ui->comboBoxTemperatureUnit->addItem(tr("F"));
    ui->comboBoxTemperatureUnit->setCurrentIndex(0);

    ui->labelPressureMaximal->setVisible(false);
    ui->labelPressureMinimal->setVisible(false);
    ui->labelTemperatureMaximal->setVisible(false);
    ui->labelTemperatureMinimal->setVisible(false);
    ui->doubleSpinBoxPressureMinimal->setVisible(false);
    ui->doubleSpinBoxPressureMaximal->setVisible(false);
    ui->doubleSpinBoxTemperatureMinimal->setVisible(false);
    ui->doubleSpinBoxTemperatureMaximal->setVisible(false);
    ui->labelPressureNormalizing->setVisible(false);
    ui->labelTemperatureNormalizing->setVisible(false);
    ui->doubleSpinBoxPressureNormalizing->setVisible(false);
    ui->doubleSpinBoxTemperatureNormalizing->setVisible(false);
}

void PagePaintCalibrationNewProject::initUi() {
    if (!mDocument) return;

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit->setMinimumDate(QDate::currentDate().addDays(-365));
    ui->dateTimeEdit->setMaximumDate(QDate::currentDate().addDays(365));
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
}

void PagePaintCalibrationNewProject::accept() {
    PspNsManager::PspTestFeaturesPaintCalibration feature;
    feature.mTestName = ui->lineEditTestName->text();
    feature.mTestModelName = ui->lineEditModelName->text();
    feature.mTestPointName = ui->lineEditPointName->text();
    feature.mTestPaintName = ui->lineEditPaintName->text();
    feature.mTestDateTime = ui->dateTimeEdit->text();
    feature.mTestTypeFormula = PspNsBase::eTypeFormula::eQuadratic;
    feature.mTestDesc = ui->textEditTestDesc->toPlainText();

    feature.mUnitPressure = (PspNsBase::eUnitPressure) ui->comboBoxPressureUnit->currentIndex();
    feature.mPressureNormalizing = ui->doubleSpinBoxPressureNormalizing->value();
    feature.mPressureMinimal = ui->doubleSpinBoxPressureMinimal->value();
    feature.mPressureMaximal = ui->doubleSpinBoxPressureMaximal->value();
    feature.mPressureTolerance = ui->doubleSpinBoxPressureTolerance->value();

    feature.mUnitTemperature = (PspNsBase::eUnitTemperature) ui->comboBoxTemperatureUnit->currentIndex();
    feature.mTemperatureNormalizing = ui->doubleSpinBoxTemperatureNormalizing->value();
    feature.mTemperatureMinimal = ui->doubleSpinBoxTemperatureMinimal->value();
    feature.mTemperatureMaximal = ui->doubleSpinBoxTemperatureMaximal->value();
    feature.mTemperatureTolerance = ui->doubleSpinBoxTemperatureTolerance->value();

    mDocument->SetTestFeatures(feature);

    QDialog::accept();
}

void PagePaintCalibrationNewProject::reject() {
    QDialog::reject();
}
