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
#include "algo/algo_transformation.h"
#include "base/base_trace.h"

using namespace cv;
using namespace PspNsBase;

namespace PspNsAlgo {
    PSP_ALGO_SHARED void resize(const Mat &src,
                                Mat &dst,
                                Size dsize,
                                double fx,
                                double fy,
                                int interpolation) {
        cv::resize(src, dst, dsize, fx, fy, interpolation);
    }

    PSP_ALGO_SHARED void applyColorMap(const Mat &src, Mat &dst, int colormap) {
        cv::applyColorMap(src, dst, colormap);
    }

    PSP_ALGO_SHARED void rotate(const Mat &src, Mat &dst, int rotateType) {
        cv::rotate(src, dst, rotateType);
    }

    PSP_ALGO_SHARED void HoughCircles(const Mat &src, Mat &dst,
                                      int method,
                                      double dp,
                                      double minDist,
                                      double param1,
                                      double param2,
                                      int minRadius,
                                      int maxRadius) {
        using namespace cv;
        using namespace std;
        src.copyTo(dst);
        vector<Vec3f> circles;
        cv::HoughCircles(src, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);
        for (size_t i = 0; i < circles.size(); i++) {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            circle(dst, center, 1, Scalar(255, 255, 0), -1, 8, 0);
            circle(dst, center, radius, Scalar(255, 0, 255), 1, 8, 0);
        }
    }

}
