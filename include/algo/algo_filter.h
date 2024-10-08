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
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

namespace PspNsAlgo {
    enum class FilterType {
        FILTER_BOX = 0,
        FILTER_MEAN,
        FILTER_MEDIAN,
        FILTER_GAUSSIAN,
        FILTER_BILATERAL
    };

    PSP_ALGO_SHARED void filterBox(const cv::Mat &src, cv::Mat &dst, int depth, cv::Size ksize,
                                   cv::Point anchor = cv::Point(-1, -1), bool normalize = true,
                                   int borderType = cv::BorderTypes::BORDER_DEFAULT);
    PSP_ALGO_SHARED void filterMean(const cv::Mat &src, cv::Mat &dst, cv::Size ksize,
                                    cv::Point anchor = cv::Point(-1, -1),
                                    int borderType = cv::BorderTypes::BORDER_DEFAULT);
    PSP_ALGO_SHARED void filterGaussian(const cv::Mat &src, cv::Mat &dst, cv::Size ksize,
                                        double sigmaX, double sigmaY = 0.0,
                                        int borderType = cv::BorderTypes::BORDER_DEFAULT);
    PSP_ALGO_SHARED void filterMedian(const cv::Mat &src, cv::Mat &dst, int ksize);
    PSP_ALGO_SHARED void filterBilateral(const cv::Mat &src, cv::Mat &dst, int d,
                                         double sigmaColor, double sigmaSpace,
                                         int borderType = cv::BorderTypes::BORDER_DEFAULT);

}
