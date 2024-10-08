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
#include "base/base_sort_points.h"

namespace PspNsBase {
    void BASE_SHARED SortPonits(vector<QPointF> &points, eBaseSortPoints sortType) {
        switch (sortType) {
            case eBaseSortPoints::eSortPointsLRTB: {
                SortPointsLRTB sortPointLRTB;
                std::sort(points.begin(), points.end(), sortPointLRTB);
                break;
            }
            case eBaseSortPoints::eSortPointsLRBT: {
                SortPointsLRBT sortPointsLRBT;
                std::sort(points.begin(), points.end(), sortPointsLRBT);
                break;
            }
            case eBaseSortPoints::eSortPointsRLTB: {
                SortPointsRLTB sortPointsRLTB;
                std::sort(points.begin(), points.end(), sortPointsRLTB);
                break;
            }
            case eBaseSortPoints::eSortPointsRLBT: {
                SortPointsRLBT sortPointsRLBT;
                std::sort(points.begin(), points.end(), sortPointsRLBT);
                break;
            }
            default: {
                break;
            }
        }
    }

    void BASE_SHARED SortMarkPonits(vector<MarkerPoint> &points, eBaseSortPoints sortType) {
        switch (sortType) {
            case eBaseSortPoints::eSortPointsLRTB: {
                SortMarkPointsLRTB sortPointLRTB;
                std::sort(points.begin(), points.end(), sortPointLRTB);
                break;
            }
            case eBaseSortPoints::eSortPointsLRBT: {
                SortMarkPointsLRBT sortPointsLRBT;
                std::sort(points.begin(), points.end(), sortPointsLRBT);
                break;
            }
            case eBaseSortPoints::eSortPointsRLTB: {
                SortMarkPointsRLTB sortPointsRLTB;
                std::sort(points.begin(), points.end(), sortPointsRLTB);
                break;
            }
            case eBaseSortPoints::eSortPointsRLBT: {
                SortMarkPointsRLBT sortPointsRLBT;
                std::sort(points.begin(), points.end(), sortPointsRLBT);
                break;
            }
            default: {
                break;
            }
        }
    }
}
