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
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"

namespace PspNsAlgo {
    PSP_ALGO_SHARED void add(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst);
    PSP_ALGO_SHARED void subtract(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst);
    PSP_ALGO_SHARED void multiply(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst);
    PSP_ALGO_SHARED void divide(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst);
    PSP_ALGO_SHARED void divide32F(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst);
    PSP_ALGO_SHARED void scaleAdd(const cv::Mat &src1, double alpha, const cv::Mat &src2, cv::Mat &dst);
    PSP_ALGO_SHARED void addWeighted(const cv::Mat &src1, double alpha, const cv::Mat &src2,
                                     double beta, double gamma, cv::Mat &dst);
    PSP_ALGO_SHARED double getPixel(const cv::Mat &src, int x, int y);
}
