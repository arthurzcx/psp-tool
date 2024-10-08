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
#include "algo/algo_2d_features.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

namespace PspNsAlgo {
    void harrisDetect(const Mat &src, Mat &dst, int blocksize, int ksize, double k, int borderType) {

        cv::cornerHarris(src, dst, blocksize, ksize, k, borderType);
    }

    PSP_ALGO_SHARED void cornerSubPixDetect(const Mat &src, Mat &dst, vector<QPointF> &points) {


        vector<KeyPoint> keyPoints;

        vector<Mat> dstes;
        split(src, dstes);
        dst = dstes[0];

        vector<Point2f> corners;
        int max_corners = 200;
        double quality_level = 0.01;
        double min_distance = 3.0;
        int block_size = 3;
        bool use_harris = false;
        double k = 0.04;

        cv::goodFeaturesToTrack(dst,
                                corners,
                                max_corners,
                                quality_level,
                                min_distance,
                                cv::Mat(),
                                block_size,
                                use_harris,
                                k);

        cv::TermCriteria criteria = cv::TermCriteria(
                cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
                40,
                0.01);

        cv::cornerSubPix(dst, corners, cv::Size(5, 5), cv::Size(-1, -1), criteria);

        for (auto iter = corners.begin(); iter != corners.end(); ++iter) {
            points.push_back(QPointF((*iter).x, (*iter).y));
            keyPoints.push_back(KeyPoint((*iter).x, (*iter).y, 3.0));
        }

        dst = src;
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

}
