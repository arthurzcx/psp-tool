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
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace PspNsAlgo {

    PSP_ALGO_SHARED void resize(const cv::Mat &src,
                                cv::Mat &dst,
                                cv::Size dsize,
                                double fx,
                                double fy,
                                int interpolation);

    PSP_ALGO_SHARED void applyColorMap(const cv::Mat &src, cv::Mat &dst, int colormap);

    PSP_ALGO_SHARED void rotate(const cv::Mat &src, cv::Mat &dst, int rotateType);

    PSP_ALGO_SHARED void HoughCircles(const cv::Mat &src, cv::Mat &dst,
                                      int method,
                                      double dp,
                                      double minDist,
                                      double param1,
                                      double param2,
                                      int minRadius,
                                      int maxRadius);

}
