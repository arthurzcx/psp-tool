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
#include "manager/work_manager.h"
#include "algo/algo_marker_detector.h"
#include "algo/algo_marker_fill.h"
#include "base/base_markerpoint_sort.h"
#include "base/base_system_param.h"
#include <QDebug>

using namespace cv;
using namespace PspNsAlgo;
using namespace PspNsBase;

namespace PspNsManager {
    WorkImgManager::WorkImgManager() {
    }

    WorkImgManager *WorkImgManager::GetInstance() {
        static WorkImgManager workImgManager;
        return &workImgManager;
    }

    void WorkImgManager::setCurImg(const Mat &mat) {
        mCurImg = mat;

        clearMarkerPoint();
        clearROI();
        emit updateMat();
    }

    const Mat &WorkImgManager::getCurImg() const {
        return mCurImg;
    }

    const std::vector<MarkerPoint> &WorkImgManager::getMarkerPoints() const {
        return mMarkerPoints;
    }

    void WorkImgManager::addMarkerPoint(const MarkerPoint &point) {
        mMarkerPoints.push_back(point);

        SortMarkPoints(mMarkerPoints, SystemParam::GetInstance()->GetSortPointType());

        emit updateMarkerPoints();
    }

    void WorkImgManager::setMarkerPoints(const std::vector<MarkerPoint> &markerPoints) {
        mMarkerPoints = markerPoints;

        SortMarkPoints(mMarkerPoints, SystemParam::GetInstance()->GetSortPointType());

        if (SystemParam::GetInstance()->IsDrawTestMarkers()) {
            for (size_t i = 0; i < mMarkerPoints.size(); i++) {
                if (i == 0) {
                    SystemParam::GetInstance()->AddTestInfo("Markers:");
                }

                QString info = "    index:" + QString::number(mMarkerPoints[i].getIndex()) + " ";
                info += "x: " + QString::number(mMarkerPoints[i].x()) + " ";
                info += "y: " + QString::number(mMarkerPoints[i].y()) + " ";
                info += "width: " + QString::number(mMarkerPoints[i].getDetectHalfWidth() * 2);
                SystemParam::GetInstance()->AddTestInfo(info);
            }
        }

        emit updateMarkerPoints();
    }

    void WorkImgManager::removeMarkerPoint(int index) {
        if (index >= 0 && index < mMarkerPoints.size()) {
            mMarkerPoints.erase(mMarkerPoints.begin() + index);
        }

        SortMarkPoints(mMarkerPoints, SystemParam::GetInstance()->GetSortPointType());

        emit updateMarkerPoints();
    }

    void WorkImgManager::clearMarkerPoint() {
        mMarkerPoints.clear();

        emit updateMarkerPoints();
    }

    void WorkImgManager::setMarkerPointSelectd(int index, bool selected) {
        if (index >= 0 && index < mMarkerPoints.size()) {
            mMarkerPoints[index].setSelected(selected);
        }

        emit updateMarkerPoints();
    }

    void WorkImgManager::replaceMarkerPoint(int index, const MarkerPoint &marker) {
        if (index >= 0 && index < mMarkerPoints.size()) {
            mMarkerPoints[index] = marker;
        }

        emit updateMarkerPoints();
    }

    void WorkImgManager::setMarkerPointAllSelected(bool selected) {
        for (auto point: mMarkerPoints) {
            point.setSelected(selected);
        }
    }

    void WorkImgManager::clearMarkerPointSelected() {
        for (auto iter = mMarkerPoints.begin(); iter != mMarkerPoints.end();) {
            if ((*iter).isSelected()) {
                iter = mMarkerPoints.erase(iter);
            } else {
                ++iter;
            }
        }

        SortMarkPoints(mMarkerPoints, SystemParam::GetInstance()->GetSortPointType());

        emit updateMarkerPoints();
    }

    void WorkImgManager::fillupMarkers() {
        if (mMarkerPoints.empty()) return;

        FillupMarkers(mCurImg, mMarkerPoints);
    }

    void WorkImgManager::detectMarkerPtByROI(const Rect &rect) {
        if (mCurImg.empty()) return;

        Mat roiMat = mCurImg(rect);

        vector<RotatedRect> ellipses;
        vector<QPointF> points;

        unsigned int radiusMarker = rect.width / 2;

        // finalise the resize scale by marker size
        double fscaleLargeMarker = 2.0;
        double fscaleSmallMarker = 10.0;
        PspNsAlgo::fitEllipse(roiMat, ellipses, points, radiusMarker,
                              radiusMarker <= 3 ? fscaleSmallMarker : fscaleLargeMarker);
        for (auto pointf: points) {
            QPointF pt(pointf.x() + rect.x, pointf.y() + rect.y);
            addMarkerPoint(MarkerPoint(mMarkerPoints.size() + 1, pt, rect.width / 2));
        }

        emit updateMarkerPoints();
    }

    void WorkImgManager::reset() {
        mMarkerPoints.clear();
        mCurImg = Mat();
        mROI.clear();
    }

    void WorkImgManager::addROI(Graphic *graphic) {
        mROI.push_back(graphic);

        emit updateROI();
    }

    void WorkImgManager::clearROI() {
        mROI.clear();

        emit updateROI();
    }

    void WorkImgManager::removeROI(int index) {
        if (index >= 0 && index < mROI.size()) {
            mROI.erase(mROI.begin() + index);
        }

        emit updateROI();
    }

    void WorkImgManager::setROI(const std::vector<Graphic *> &graphics) {
        mROI = graphics;

        emit updateROI();
    }

    const std::vector<Graphic *> &WorkImgManager::getROI() const {
        return mROI;
    }
}
