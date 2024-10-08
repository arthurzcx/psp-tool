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
#include "algo/algo_filter.h"
#include "base/base_trace.h"

using namespace PspNsBase;
using namespace cv;

namespace PspNsAlgo {
    void filterBox(const Mat &src, Mat &dst, int depth, cv::Size ksize,
                   cv::Point anchor, bool normalize, int borderType) {

        boxFilter(src, dst, depth, ksize, anchor, normalize, borderType);
    }

    void filterMean(const Mat &src, Mat &dst, cv::Size ksize,
                    cv::Point anchor, int borderType) {

        blur(src, dst, ksize, anchor, borderType);
    }

    void filterGaussian(const Mat &src, Mat &dst, cv::Size ksize,
                        double sigmaX, double sigmaY, int borderType) {

        GaussianBlur(src, dst, ksize, sigmaX, sigmaY, borderType);
    }

    void filterMedian(const Mat &src, Mat &dst, int ksize) {

        medianBlur(src, dst, ksize);
    }

    void filterBilateral(const Mat &src, Mat &dst, int d,
                         double sigmaColor, double sigmaSpace, int borderType) {

        bilateralFilter(src, dst, d, sigmaColor, sigmaSpace, borderType);
    }

}
