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

#include <QDialog>
#include <QQuickItem>

namespace Ui {
    class PageTwoImageCalculation;
}

enum eTypeTwoImageCalculation {
    eAddition = 0,
    eSubtract,
    eDivision
};


class LogicTwoImageCalculation : public QObject {
Q_OBJECT

public:
    LogicTwoImageCalculation();
    ~LogicTwoImageCalculation();

    Q_INVOKABLE void selectFirstImage();
    Q_INVOKABLE void selectSecondImage();

    Q_INVOKABLE void emitReject();
    Q_INVOKABLE void emitAccept();

    void calculation(eTypeTwoImageCalculation type);

signals:
    void updateFirstImage(const QString &firstImagePath);
    void updateSecondImage(const QString &secondImagePath);

    void reject();
    void accept();

private:
    bool selectImage(bool isFirst);

private:
    QString mFirstImagePath;
    QString mSecondImagePath;

    QString mPlatformPrex;
};

class PageTwoImageCalculation : public QDialog {
Q_OBJECT

public:
    explicit PageTwoImageCalculation(QWidget *parent = nullptr,
                                     eTypeTwoImageCalculation type = eTypeTwoImageCalculation::eAddition);
    ~PageTwoImageCalculation();

public slots:
    void accept();

private:
    Ui::PageTwoImageCalculation *ui;
    eTypeTwoImageCalculation mTypeCalculation;

    LogicTwoImageCalculation *logicTwoImageCalculation;
    QQuickItem *mQuickItem;
};

