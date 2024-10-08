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
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

namespace PspNsAlgo {

    PSP_ALGO_SHARED void BasicThreshold(const cv::Mat &src, cv::Mat &dst, double thresh, double maxval, int threshType);


    PSP_ALGO_SHARED void
    adaptiveThreshold(const cv::Mat &src, cv::Mat &dst, double maxValue, int adaptiveMethod, int thresholdType,
                      int blocksize, double constValue);

    PSP_ALGO_SHARED void calcAdapiveThreshold(const cv::Mat &src, double &lowThreshold, double &hightThreshold);

    PSP_ALGO_SHARED void calcOstuThreshold(const cv::Mat &src, double &lowThreshold, double &hightThreshold);

    PSP_ALGO_SHARED void calcTriangleThreshold(const cv::Mat &src, double &lowThreshold, double &highThreshold);

    PSP_ALGO_SHARED void calcHistThreshold(const cv::Mat &src, double &lowThreshold, double &highThreshold);

    PSP_ALGO_SHARED void thresholdSegmentation(cv::Mat &src, double threshold);
}
