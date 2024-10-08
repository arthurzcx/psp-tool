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
#include "base/base_markerpoint.h"
#include "base/base_def.h"
#include "base/base_trace.h"
#include <assert.h>
#include <math.h>

namespace PspNsBase {

    MarkerPoint::MarkerPoint()
            : mPoint(MAX_VALUE, MAX_VALUE), mIndex(-1), mActive(0), mDetectHalfWidth(3), mSelected(false) {
    }

    MarkerPoint::MarkerPoint(int index)
            : mPoint(MAX_VALUE, MAX_VALUE), mIndex(index), mActive(0), mDetectHalfWidth(3), mSelected(false) {
    }

    MarkerPoint::MarkerPoint(const QPointF &point)
            : mPoint(point), mIndex(-1), mActive(0), mDetectHalfWidth(3), mSelected(false) {
    }

    MarkerPoint::MarkerPoint(int index, const QPointF &point)
            : mPoint(point), mIndex(index), mActive(0), mDetectHalfWidth(3), mSelected(false) {
    }

    MarkerPoint::MarkerPoint(const QPointF &point, int detectHalfWidth)
            : mPoint(point), mIndex(-1), mActive(0), mDetectHalfWidth(detectHalfWidth), mSelected(false) {
    }

    MarkerPoint::MarkerPoint(int index, const QPointF &point, int detectHalfWidth)
            : mPoint(point), mIndex(index), mActive(0), mDetectHalfWidth(detectHalfWidth), mSelected(false) {
    }

    MarkerPoint::MarkerPoint(int index, const QPointF &point, int active, int detectHalfWidth)
            : mPoint(point), mIndex(index), mActive(active), mDetectHalfWidth(detectHalfWidth), mSelected(false) {

    }

    MarkerPoint::MarkerPoint(const MarkerPoint &markerPoint)
            : mPoint(markerPoint.mPoint), mIndex(markerPoint.mIndex), mActive(markerPoint.mActive),
              mDetectHalfWidth(markerPoint.mDetectHalfWidth), mSelected(markerPoint.mSelected) {

    }

    MarkerPoint &MarkerPoint::operator=(const MarkerPoint &markerPoint) {
        mPoint = markerPoint.mPoint;
        mIndex = markerPoint.mIndex;
        mActive = markerPoint.mActive;
        mDetectHalfWidth = markerPoint.mDetectHalfWidth;
        mSelected = markerPoint.mSelected;

        return *this;
    }

    void MarkerPoint::setIndex(int index) {
        mIndex = index;
    }

    void MarkerPoint::setActive(int active) {
        if (active != 0 && active != 1) {
            assert(false);
        }

        mActive = active;
    }

    int MarkerPoint::getActive() const {
        return mActive;
    }

    void MarkerPoint::setPoint(const QPointF &point) {
        mPoint = point;
    }

    void MarkerPoint::setDetectHalfWidth(int width) {
        mDetectHalfWidth = width;
    }

    void MarkerPoint::setSelected(bool selected) {
        mSelected = selected;
    }

    int MarkerPoint::getIndex() const {
        return mIndex;
    }
    const QPointF &MarkerPoint::getPoint() const {
        return mPoint;
    }

    int MarkerPoint::getDetectHalfWidth() const {
        return mDetectHalfWidth;
    }

    void MarkerPoint::setX(double x) {
        mPoint.setX(x);
    }

    void MarkerPoint::setY(double y) {
        mPoint.setY(y);
    }

    double MarkerPoint::x() const {
        return mPoint.x();
    }
    double MarkerPoint::y() const {
        return mPoint.y();
    }

    bool MarkerPoint::isSelected() const {
        return mSelected;
    }

    bool MarkerPoint::isValid() const {
        if (fabs(mPoint.x() - MAX_VALUE) < EPSILON || fabs(mPoint.y() - MAX_VALUE) < EPSILON || mIndex < 0 ||
            (mActive != 0 && mActive != 1))
            return false;

        return true;
    }

    BASE_SHARED void to_json(Json &j, const MarkerPoint &marker) {
        j["index"] = marker.getIndex();
        j["x"] = marker.x();
        j["y"] = marker.y();
        j["halfWidth"] = marker.getDetectHalfWidth();
        j["active"] = marker.getActive();
        j["selected"] = marker.isSelected();
    }

    BASE_SHARED void from_json(const Json &j, MarkerPoint &marker) {
        if (j.contains("index")) {
            marker.setIndex(j["index"]);
        }

        if (j.contains("x")) {
            marker.setX(j["x"]);
        }

        if (j.contains("y")) {
            marker.setY(j["y"]);
        }

        if (j.contains("halfWidth")) {
            marker.setDetectHalfWidth(j["halfWidth"]);
        }

        if (j.contains("selected")) {
            marker.setSelected(j["selected"]);
        }

        if (j.contains("active")) {
            marker.setActive(j["active"]);
        }
    }

}
