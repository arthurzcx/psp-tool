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
#include "dialog_auto_sort_makers.h"
#include "ui_dialog_auto_sort_makers.h"
#include "base/base_system_param.h"

Dialog_AutoSortMakers::Dialog_AutoSortMakers(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog_AutoSortMakers) {
    ui->setupUi(this);

    translateUi();
}

Dialog_AutoSortMakers::~Dialog_AutoSortMakers() {
    delete ui;
}


void Dialog_AutoSortMakers::translateUi() {
    ui->comboBoxAutoSort->clear();

    QStringList items;
    items << tr("Left to right, top to bottom");
    items << tr("Left to right, bottom to top");
    items << tr("Right to left, top to bottom");
    items << tr("Right to left, bottom to top");

    ui->comboBoxAutoSort->addItems(items);

    int methodSortPoint = (int) PspNsBase::SystemParam::GetInstance()->GetSortPointType();
    if (methodSortPoint >= 0 || methodSortPoint < 4) {
        ui->comboBoxAutoSort->setCurrentIndex(methodSortPoint);
    }
}

int Dialog_AutoSortMakers::GetMethodIndex() {
    return ui->comboBoxAutoSort->currentIndex();
}
