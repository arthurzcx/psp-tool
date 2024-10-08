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
#include "page_frequency.h"
#include "ui_page_frequency.h"
#include <QPixmap>
#include <QLabel>

PageFrequency::PageFrequency(QImage img, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::PageFrequency) {
    ui->setupUi(this);

    QPixmap pixmap = QPixmap::fromImage(img);
    int with = ui->labelImageFrequency->width();
    int height = ui->labelImageFrequency->height();
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelImageFrequency->setPixmap(fitpixmap);
}

PageFrequency::~PageFrequency() {
    delete ui;
}
