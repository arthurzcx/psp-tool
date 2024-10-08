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
#include "psp_system_config_dialog.h"
#include "ui_systemconfigdialog.h"
#include "base/base_system_param.h"
#include <QDebug>
#include <QColorDialog>

using namespace PspNsBase;

SystemConfigDialog::SystemConfigDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SystemConfigDialog) {
    ui->setupUi(this);

    init();
}

SystemConfigDialog::~SystemConfigDialog() {
    delete ui;
}

void SystemConfigDialog::init() {
    setBgColorButton(SystemParam::GetInstance()->GetBackgroundColor());
    ui->comboBoxDisplayMode->setCurrentIndex(SystemParam::GetInstance()->IsShowPseudoColor() ? 0 : 1);

    connect(ui->buttonBgColor, SIGNAL(clicked(bool)), this, SLOT(configBgColor()));

    ui->comboBoxSortPointType->clear();

    QStringList items;
    items << tr("Left to right, top to bottom");
    items << tr("Left to right, bottom to top");
    items << tr("Right to left, top to bottom");
    items << tr("Right to left, bottom to top");

    ui->comboBoxSortPointType->addItems(items);

    int methodSortPoint = (int) PspNsBase::SystemParam::GetInstance()->GetSortPointType();
    if (methodSortPoint >= 0 || methodSortPoint < 4) {
        ui->comboBoxSortPointType->setCurrentIndex(methodSortPoint);
    }
}

void SystemConfigDialog::setBgColorButton(int color) {
    QString bgColorStr;
    bgColorStr.setNum(color, 16);
    if (bgColorStr.size() > 8)
        bgColorStr.replace("ffffffff", "");

    if (bgColorStr.isEmpty()) return;

    QString styleSheet(QString("QPushButton{color:tranparent;background-color:#%1} \
        QPushButton:focus{outline:none;}").arg(bgColorStr));
    ui->buttonBgColor->setStyleSheet(styleSheet);
    mBackgroundColor = color;
}

int SystemConfigDialog::getBgColor() {
    return mBackgroundColor;
}

void SystemConfigDialog::configBgColor() {
    QColorDialog colorDialog;
    if (colorDialog.exec() == QDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        if (selectedColor.isValid())
            setBgColorButton(selectedColor.rgb());
    }
}

void SystemConfigDialog::on_buttonBox_accepted() {
    SystemParam::GetInstance()->SetIsShowPseudoColor(ui->comboBoxDisplayMode->currentIndex() == 0);
}
