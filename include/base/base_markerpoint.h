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
#include "base_global.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace PspNsBase {

    class BASE_SHARED MarkerPoint {
    public:
        MarkerPoint();
        MarkerPoint(int index);
        MarkerPoint(const QPointF &point);
        MarkerPoint(int index, const QPointF &point);
        MarkerPoint(const QPointF &point, int detectHalfWidth);
        MarkerPoint(int index, const QPointF &point, int detectHalfWidth);
        MarkerPoint(int index, const QPointF &point, int active, int detectHalfWidth);

        MarkerPoint(const MarkerPoint &markerPoint);
        MarkerPoint &operator=(const MarkerPoint &markerPoint);

        void setIndex(int index);
        void setActive(int active);
        void setPoint(const QPointF &point);
        void setDetectHalfWidth(int width);
        void setSelected(bool selected);

        int getIndex() const;
        int getActive() const;
        const QPointF &getPoint() const;
        int getDetectHalfWidth() const;
        bool isSelected() const;

        void setX(double x);
        void setY(double y);
        double x() const;
        double y() const;

        bool isValid() const;

    private:
        QPointF mPoint;
        int mIndex;
        int mActive;
        int mDetectHalfWidth;
        bool mSelected;
    };

    BASE_SHARED void to_json(Json &j, const MarkerPoint &marker);
    BASE_SHARED void from_json(const Json &j, MarkerPoint &marker);
}
