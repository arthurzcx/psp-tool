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

namespace Ui {
    class PagePseudoConfig;
}

class PagePseudoConfig : public QDialog {
Q_OBJECT

public:
    explicit PagePseudoConfig(QWidget *parent = nullptr);
    ~PagePseudoConfig();

    void Init();

    const QImage &GetResultImage() const;

signals:
    void signalMinSpinValueChanged(double);
    void signalMaxSpinValueChanged(double);
    void signalMinSliderValueChanged(double);
    void signalMaxSliderValueChanged(double);

public slots:
    void accept();

    void slotMinSpinValueChanged(double);
    void slotMaxSpinValueChanged(double);
    void slotMinSliderValueChanged();
    void slotMaxSliderValueChanged();

private:
    Ui::PagePseudoConfig *ui;

    int mMinVal;
    int mMaxVal;
    int mSliderRange;

    double mMinRealVal;
    double mMaxRealVal;
    double mStep;

    QImage mResultImage;
};

