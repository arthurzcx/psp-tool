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
#include "dlg_op_marker_add_size.h"
#include "ui_dlg_op_marker_add_size.h"

Dialog_OperatorMarkerAdd_Size::Dialog_OperatorMarkerAdd_Size(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Dialog_OperatorMarkerAdd_Size) {
    ui->setupUi(this);
}

Dialog_OperatorMarkerAdd_Size::~Dialog_OperatorMarkerAdd_Size() {
    delete ui;
}

int Dialog_OperatorMarkerAdd_Size::GetBoxSize() {
    return ui->spinBoxBoxSize->value();
}

void Dialog_OperatorMarkerAdd_Size::SetBoxSize(int size) {
    ui->spinBoxBoxSize->setValue(size);
}
