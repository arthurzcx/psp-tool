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
#include "base/base_graphic_polygon.h"
#include <functional>
#include <float.h>
#include <cmath>

using namespace std;

namespace PspNsBase {
    struct SortPointsXminYmin__ : public binary_function<QPointF, QPointF, bool> {
        explicit SortPointsXminYmin__() {}
        bool operator()(const QPointF &a, const QPointF &b) const {
            return a.x() < b.x() || (fabs(a.x() - b.x()) < FLT_EPSILON && a.y() < b.y());
        }
    };

    GraphicPolygon::GraphicPolygon()
            : Graphic() {

    }

    GraphicPolygon::~GraphicPolygon() {

    }

    GraphicPolygon::GraphicPolygon(const std::vector<QPointF> &points)
            : Graphic(), mPoints(points) {

    }

    GraphicPolygon::GraphicPolygon(const GraphicPolygon &polygon)
            : Graphic(polygon), mPoints(polygon.mPoints) {

    }

    GraphicPolygon &GraphicPolygon::operator=(const GraphicPolygon &polygon) {
        mPoints = polygon.mPoints;
        return *this;
    }

    void GraphicPolygon::AddPoint(const QPointF &point) {
        mPoints.push_back(point);
    }

    const std::vector<QPointF> &GraphicPolygon::GetPoints() const {
        return mPoints;
    }

    void GraphicPolygon::RemovePoint(int index) {
        if (index >= 0 && index < mPoints.size()) {
            mPoints.erase(mPoints.begin() + index);
        }
    }

    static double IsLeft(const QPointF &P0, const QPointF &P1, const QPointF &P2) {
        return (P1.x() - P0.x()) * (P2.y() - P0.y()) - (P2.x() - P0.x()) * (P1.y() - P0.y());
    }

    void GraphicPolygon::MakeConvex() {
        if (mPoints.size() < 3) return;

        SortPointsXminYmin__ sortPoints;
        sort(mPoints.begin(), mPoints.end(), sortPoints);

        int n = mPoints.size();

        vector<QPointF> pointsConvex;
        pointsConvex.resize(n + 1);

        int top = -1;

        int i = 0, minmin = 0, minmax = 0;
        double xmin = mPoints[0].x();
        for (i = 1; i < n; i++) {
            if (fabs(mPoints[i].x() - xmin) > FLT_EPSILON)
                break;
        }

        minmax = i - 1;
        if (minmax == n - 1) {
            pointsConvex[++top] = mPoints[minmin];

            if (fabs(mPoints[minmax].y() - mPoints[minmin].y()) > FLT_EPSILON) {
                pointsConvex[++top] = mPoints[minmax];
            }

            pointsConvex[++top] = mPoints[minmin];

            pointsConvex.erase(pointsConvex.begin() + top + 1, pointsConvex.end());

            if (pointsConvex[0] == pointsConvex[pointsConvex.size() - 1])
                pointsConvex.pop_back();
            return;
        }

        int maxmin = 0, maxmax = n - 1;
        double xmax = mPoints[n - 1].x();
        for (i = n - 2; i >= 0; i--) {
            if (fabs(mPoints[i].x() - xmax) > FLT_EPSILON)
                break;
        }
        maxmin = i + 1;

        pointsConvex[++top] = mPoints[minmin];
        i = minmax;
        while (++i <= maxmin) {
            if (IsLeft(mPoints[minmin], mPoints[maxmin], mPoints[i]) > 0 && i < maxmin)
                continue;

            while (top > 0) {
                if (IsLeft(pointsConvex[top - 1], pointsConvex[top], mPoints[i]) > 0)
                    break;
                else
                    top--;
            }

            pointsConvex[++top] = mPoints[i];
        }

        if (maxmax != maxmin)
            pointsConvex[++top] = mPoints[maxmax];

        int bot = top;
        i = maxmin;

        while (--i >= minmax) {
            if (IsLeft(mPoints[maxmax], mPoints[minmax], mPoints[i]) > 0 && i > minmax)
                continue;

            while (top > bot) {
                if (IsLeft(pointsConvex[top - 1], pointsConvex[top], mPoints[i]) > 0) {
                    break;
                } else {
                    top--;
                }
            }

            if (mPoints[i] == pointsConvex[0]) {
                pointsConvex.erase(pointsConvex.begin() + top + 1, pointsConvex.end());

                if (pointsConvex[0] == pointsConvex[pointsConvex.size() - 1]) {
                    pointsConvex.pop_back();
                }

                mPoints = pointsConvex;
                return;
            }

            pointsConvex[++top] = mPoints[i];
        }

        if (minmax != minmin) {
            pointsConvex[++top] = mPoints[minmin];
        }

        pointsConvex.erase(pointsConvex.begin() + top + 1, pointsConvex.end());

        if (pointsConvex[0] == pointsConvex[pointsConvex.size() - 1]) {
            pointsConvex.pop_back();
        }

        mPoints = pointsConvex;
    }

    bool GraphicPolygon::IsValid() {
        MakeConvex();

        if (mPoints.size() < 3) {
            return false;
        }

        return true;
    }

    bool GraphicPolygon::IsInRect(const GraphicRect &rect) {
        for (auto point: mPoints) {
            if (!rect.ContainPoint(point)) {
                return false;
            }
        }

        return true;
    }

    BASE_SHARED void to_json(Json &j, const PspNsBase::GraphicPolygon &polygon) {
        j["type"] = "polygon";
        const std::vector<QPointF> &points = polygon.GetPoints();
        for (auto point: points) {
            j["data"].push_back(point.x());
            j["data"].push_back(point.y());
        }
    }

    BASE_SHARED void from_json(const Json &j, PspNsBase::GraphicPolygon &polygon) {
        if (j["data"].is_array()) {
            if (j["data"].size() % 2 != 0) assert(false);

            for (size_t i = 0; i < j["data"].size(); i = i + 2) {
                polygon.AddPoint(QPointF(j["data"].at(i), j["data"].at(i + 1)));
            }
        }
    }
}
