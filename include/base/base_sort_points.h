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

#include <QPointF>
#include <functional>
#include <math.h>
#include <float.h>
#include <vector>
#include "base_markerpoint.h"
#include "base_global.h"

using namespace std;

namespace PspNsBase {
#define PRECISION_PIXEL 25

    enum eBaseSortPoints {
        eSortPointsLRTB = 0,
        eSortPointsLRBT,
        eSortPointsRLTB,
        eSortPointsRLBT
    };

    struct SortPointsLRTB : public binary_function<QPointF, QPointF, bool> {
        explicit SortPointsLRTB() {}

        bool operator()(const QPointF &pt1, const QPointF &pt2) {
            if (pt1.x() - pt2.x() < -PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() < PRECISION_PIXEL && pt1.y() - pt2.y() < 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct SortPointsLRBT : public binary_function<QPointF, QPointF, bool> {
        explicit SortPointsLRBT() {}

        bool operator()(const QPointF &pt1, const QPointF &pt2) {
            if (pt1.x() - pt2.x() < -PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() < PRECISION_PIXEL && pt1.y() - pt2.y() > 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct SortPointsRLTB : binary_function<QPointF, QPointF, bool> {
        explicit SortPointsRLTB() {}

        bool operator()(const QPointF &pt1, const QPointF &pt2) {
            if (pt1.x() - pt2.x() > PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() > -PRECISION_PIXEL && pt1.y() - pt2.y() < 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct SortPointsRLBT : public binary_function<QPointF, QPointF, bool> {
        explicit SortPointsRLBT() {}

        bool operator()(const QPointF &pt1, const QPointF &pt2) {
            if (pt1.x() - pt2.x() > PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() > -PRECISION_PIXEL && pt1.y() - pt2.y() > 0) {
                return true;
            } else {
                return false;
            }
        }
    };


    struct SortMarkPointsLRTB : public binary_function<MarkerPoint, MarkerPoint, bool> {
        explicit SortMarkPointsLRTB() {}

        bool operator()(const MarkerPoint &pt1, const MarkerPoint &pt2) {
            if (pt1.x() - pt2.x() < -PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() < PRECISION_PIXEL && pt1.y() - pt2.y() < 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct SortMarkPointsLRBT : public binary_function<MarkerPoint, MarkerPoint, bool> {
        explicit SortMarkPointsLRBT() {}

        bool operator()(const MarkerPoint &pt1, const MarkerPoint &pt2) {
            if (pt1.x() - pt2.x() < -PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() < PRECISION_PIXEL && pt1.y() - pt2.y() > 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct SortMarkPointsRLTB : public binary_function<MarkerPoint, MarkerPoint, bool> {
        explicit SortMarkPointsRLTB() {}

        bool operator()(const MarkerPoint &pt1, const MarkerPoint &pt2) {
            if (pt1.x() - pt2.x() > PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() > -PRECISION_PIXEL && pt1.y() - pt2.y() < 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct SortMarkPointsRLBT : public binary_function<MarkerPoint, MarkerPoint, bool> {
        explicit SortMarkPointsRLBT() {}

        bool operator()(const MarkerPoint &pt1, const MarkerPoint &pt2) {
            if (pt1.x() - pt2.x() > PRECISION_PIXEL) {
                return true;
            } else if (pt1.x() - pt2.x() > -PRECISION_PIXEL && pt1.y() - pt2.y() > 0) {
                return true;
            } else {
                return false;
            }
        }
    };

    void BASE_SHARED SortPonits(vector <QPointF> &points, eBaseSortPoints sortType = eSortPointsLRTB);
    void BASE_SHARED SortMarkPonits(vector <MarkerPoint> &points, eBaseSortPoints sortType = eSortPointsLRTB);
}
