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
#include "page_project_features.h"
#include "ui_page_project_features.h"
#include "manager/manager_doc_single_component.h"

PageProjectFeatures::PageProjectFeatures(QWidget *parent, PspNsManager::PspDocument *pDoc) :
        QDialog(parent),
        ui(new Ui::PageProjectFeatures),
        mDocument(pDoc) {
    ui->setupUi(this);

    Init();
}

PageProjectFeatures::~PageProjectFeatures() {
    delete ui;
}

void PageProjectFeatures::Init() {
    ui->comboBoxPressureUnit->addItem(tr("Pa"));
    ui->comboBoxPressureUnit->addItem(tr("KPa"));
    ui->comboBoxPressureUnit->setCurrentIndex(0);

    ui->comboBoxTemperatureUnit->addItem(tr("K"));
    ui->comboBoxTemperatureUnit->addItem(tr("C"));
    ui->comboBoxTemperatureUnit->setCurrentIndex(0);

    TranslateUi();

    if (PspNsManager::PspDocumentSingleComponent *doc = dynamic_cast<PspNsManager::PspDocumentSingleComponent *>(mDocument)) {
        ui->comboBoxPressureUnit->setCurrentIndex((int) (doc->GetTestFeaturesPressureUnit()));
        ui->comboBoxTemperatureUnit->setCurrentIndex((int) (doc->GetTestFeaturesTemperatureUnit()));
        ui->doubleSpinBoxPressureStatic->setValue(doc->GetTestFeaturesStaticPressure());
        ui->doubleSpinBoxPressureDynamic->setValue(doc->GetTestFeaturesDynamicPressure());
        ui->doubleSpinBoxPressureWindOff->setValue(doc->GetTestFeaturesWindOffPressure());
        ui->doubleSpinBoxTemperatureWindOff->setValue(doc->GetTestFeaturesWindOffTemperature());
        ui->doubleSpinBoxTemperatureWindOn->setValue(doc->GetTestFeaturesWindOnTemperature());
        ui->lineEditTestId->setText(doc->GetTestFeaturesTestId());
        ui->lineEditModelId->setText(doc->GetTestFeaturesModelId());
        ui->lineEditTestPointId->setText(doc->GetTestFeaturesTestPointId());
        ui->doubleSpinBoxMach->setValue(doc->GetTestFeaturesMach());
        ui->doubleSpinBoxAlpha->setValue(doc->GetTestFeaturesAlpha());
        ui->doubleSpinBoxBeta->setValue(doc->GetTestFeaturesBeta());
    }
}

void PageProjectFeatures::TranslateUi() {
    ui->labelAlpha->setText(tr("Alpha"));
    ui->labelBeta->setText(tr("Beta"));
    ui->labelMach->setText(tr("Mach"));
    ui->labelModelId->setText(tr("Model id"));
    ui->labelPressureDynamic->setText(tr("Pressure dynamic"));
    ui->labelPressureStatic->setText(tr("Pressure static"));
    ui->labelPressureUnit->setText(tr("Pressure unit"));
    ui->labelPressureWindOff->setText(tr("Pressure wind off"));
    ui->labelTemperatureOn->setText(tr("Temperature wind on"));
    ui->labelTemperatureUnit->setText(tr("Temperature unit"));
    ui->labelTemperatureWindOff->setText(tr("Temperature wind off"));
    ui->labelTestId->setText(tr("Test id"));
    ui->labelTestPointId->setText(tr("Test point id"));
}

void PageProjectFeatures::accept() {
    if (PspNsManager::PspDocumentSingleComponent *doc = dynamic_cast<PspNsManager::PspDocumentSingleComponent *>(mDocument)) {
        doc->SetTestFeaturesPressureUnit((eUnitPressure) (ui->comboBoxPressureUnit->currentIndex()));
        doc->SetTestFeaturesTemperatureUnit((eUnitTemperature) (ui->comboBoxTemperatureUnit->currentIndex()));
        doc->SetTestFeaturesStaticPressure(ui->doubleSpinBoxPressureStatic->value());
        doc->SetTestFeaturesDynamicPressure(ui->doubleSpinBoxPressureDynamic->value());
        doc->SetTestFeaturesWindOffPressure(ui->doubleSpinBoxPressureWindOff->value());
        doc->SetTestFeaturesWindOffTemperature(ui->doubleSpinBoxTemperatureWindOff->value());
        doc->SetTestFeaturesWindOnTemperature(ui->doubleSpinBoxTemperatureWindOn->value());
        doc->SetTestFeaturesTestId(ui->lineEditTestId->text());
        doc->SetTestFeaturesModelId(ui->lineEditModelId->text());
        doc->SetTestFeaturesTestPointId(ui->lineEditTestPointId->text());
        doc->SetTestFeaturesMach(ui->doubleSpinBoxMach->value());
        doc->SetTestFeaturesAlpha(ui->doubleSpinBoxAlpha->value());
        doc->SetTestFeaturesBeta(ui->doubleSpinBoxBeta->value());
    }

    QDialog::accept();
}
