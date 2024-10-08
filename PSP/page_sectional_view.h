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

#include <QWidget>
#include <QDialog>
#include <vector>

namespace Ui {
    class PageSectionalView;
}

enum eSectionalViewType {
    eVert = 0,
    eHoriz,
    eBilateral
};

class PageSectionalView : public QDialog {
Q_OBJECT

public:
    explicit PageSectionalView(eSectionalViewType type,
                               int minX, int maxX, int minY, int maxY,
                               const std::vector<double> &valueX,
                               const std::vector<double> &valueY,
                               QWidget *parent = nullptr);
    ~PageSectionalView();

private:
    bool ParseData();

    void InitChart();

private:
    Ui::PageSectionalView *ui;

    eSectionalViewType mType;
    int mMinX;
    int mMaxX;
    int mMinY;
    int mMaxY;

    double mMinValX;
    double mMaxValX;
    double mMinValY;
    double mMaxValY;

    std::vector<double> mValX;
    std::vector<double> mValY;
};
