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

#include "manager_global.h"
#include "base/base_markerpoint.h"
#include "base/base_graphic.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <QObject>

using namespace PspNsBase;

namespace PspNsManager {
    class MANAGER_SHARED WorkImgManager : public QObject {
    Q_OBJECT

        WorkImgManager();

    public:
        static WorkImgManager *GetInstance();

        void setCurImg(const cv::Mat &mat);
        const cv::Mat &getCurImg() const;

        const std::vector<MarkerPoint> &getMarkerPoints() const;
        void addMarkerPoint(const MarkerPoint &point);
        void setMarkerPoints(const std::vector<MarkerPoint> &markerPoints);
        void removeMarkerPoint(int index);
        void clearMarkerPoint();
        void setMarkerPointSelectd(int index, bool selected);
        void replaceMarkerPoint(int index, const MarkerPoint &marker);
        void setMarkerPointAllSelected(bool selected);
        void clearMarkerPointSelected();
        void fillupMarkers();

        void addROI(Graphic *graphic);
        void clearROI();
        void removeROI(int index);
        void setROI(const std::vector<Graphic *> &graphics);
        const std::vector<Graphic *> &getROI() const;

        void detectMarkerPtByROI(const cv::Rect &rect);

        void reset();
    signals:
        void updateMat();
        void updateMarkerPoints();
        void updateWorkInfo();

        void updateROI();

    private:
        cv::Mat mCurImg;
        cv::Mat mSrcImg;
        std::vector<MarkerPoint> mMarkerPoints;
        std::vector<Graphic *> mROI;
    };
}
