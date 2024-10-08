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
#include <vector>
#include <QPointF>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"

using namespace std;

namespace PspNsAlgo {
    PSP_ALGO_SHARED void removeTooClosePoints(vector<cv::KeyPoint> &points, float distance = 5);
    PSP_ALGO_SHARED void removeTooClosePoints(vector<QPointF> &points, float distance = 5);
    PSP_ALGO_SHARED void removeTooCloseEllipses(vector<cv::RotatedRect> &ellipses, float distance = 5);
    PSP_ALGO_SHARED void removeInvalidPoints(vector<QPointF> &points, int xMax, int yMax);
    PSP_ALGO_SHARED void removeInvalidEllipses(vector<cv::RotatedRect> &ellipses, int xMax, int yMax);

    PSP_ALGO_SHARED void fastDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void agastDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void akazeDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void briskDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void gfttDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void kazeDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void mserDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void orbDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);
    PSP_ALGO_SHARED void simpleBlobDetector(const cv::Mat &src, vector<QPointF> &points, int threshold, cv::Mat &dst);

    PSP_ALGO_SHARED void fitEllipse(const cv::Mat &src, vector<cv::RotatedRect> &ellipses, vector<QPointF> &points,
                                    unsigned int radiusMarker = 5, double fscale = 2.0);
}
