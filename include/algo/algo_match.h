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

#include "algo_global.h"
#include "base/base_markerpoint.h"
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <vector>

using namespace std;
using namespace PspNsBase;

namespace PspNsAlgo {
    PSP_ALGO_SHARED void
    SurfMatch(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst, int threshold1 = 40, int threshold2 = 40);

    PSP_ALGO_SHARED void
    FindHomography(const vector <cv::Point2f> &kp1, const vector <cv::Point2f> &kp2, cv::Mat &homographyMatrix,
                   int methodType = 0);

    PSP_ALGO_SHARED void
    Match(const cv::Mat &src1, const cv::Mat &src2, vector <cv::KeyPoint> &kp1, vector <cv::KeyPoint> &kp2,
          cv::Mat &homographyMatrix, cv::Mat &alignedDst);

    PSP_ALGO_SHARED bool
    FindHasSameRadiusEllipse(const vector <cv::RotatedRect> &ellipses, const MarkerPoint &mp,
                             cv::RotatedRect &resultEllipse);
    PSP_ALGO_SHARED bool
    MatchTemplate(const cv::Mat &srcTemplate, const cv::Mat &srcMatch, const MarkerPoint &kpTemplate,
                  qreal searchRadius, MarkerPoint &kpMatch, int threshold);
}
