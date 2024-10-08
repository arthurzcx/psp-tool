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
#include "algo/algo_threshold.h"
#include <QDebug>

using namespace cv;

namespace PspNsAlgo {

    PSP_ALGO_SHARED void BasicThreshold(const Mat &src, Mat &dst, double thresh, double maxval, int threshType) {

        cv::threshold(src, dst, thresh, maxval, threshType);
    }

    PSP_ALGO_SHARED void
    adaptiveThreshold(const Mat &src, Mat &dst, double maxValue, int adaptiveMethod, int thresholdType, int blockSize,
                      double constValue) {

        cv::adaptiveThreshold(src, dst, maxValue, adaptiveMethod, thresholdType, blockSize, constValue);
    }


    PSP_ALGO_SHARED void calcAdapiveThreshold(const cv::Mat &src, double &lowThreshold, double &highThreshold) {
        double meanVal = cv::mean(src)[0];
        lowThreshold = meanVal * 0.45;
        highThreshold = meanVal * 1.2;

    }

    PSP_ALGO_SHARED void calcOstuThreshold(const cv::Mat &src, double &lowThreshold, double &highThreshold) {

        cv::Mat threshedMat;
        lowThreshold = cv::threshold(src, threshedMat, 0, 255, THRESH_OTSU);
        highThreshold = 2 * lowThreshold;
    }

    PSP_ALGO_SHARED void calcTriangleThreshold(const cv::Mat &src, double &lowThreshold, double &highThreshold) {

        cv::Mat threshedMat;
        lowThreshold = cv::threshold(src, threshedMat, 0, 255, THRESH_TRIANGLE);
        highThreshold = 2 * lowThreshold;
    }

    PSP_ALGO_SHARED void calcHistThreshold(const cv::Mat &dx, double &lowThreshold, double &highThreshold) {
    }

    PSP_ALGO_SHARED void thresholdSegmentation(cv::Mat &src, double threshold) {
        if (src.empty() || threshold < FLT_EPSILON) return;

        switch (src.type()) {
            case CV_8U: {
                for (int i = 0; i < src.rows; i++) {
                    for (int j = 0; j < src.cols; j++) {
                        if (src.at<uchar>(i, j) < threshold) {
                            src.at<uchar>(i, j) = 0;
                        }
                    }
                }
                break;
            }
            case CV_16U: {
                for (int i = 0; i < src.rows; i++) {
                    for (int j = 0; j < src.cols; j++) {
                        if (src.at<ushort>(i, j) < threshold) {
                            src.at<ushort>(i, j) = 0;
                        }
                    }
                }
                break;
            }
            case CV_32F: {
                for (int i = 0; i < src.rows; i++) {
                    for (int j = 0; j < src.cols; j++) {
                        if (src.at<float>(i, j) < threshold) {
                            src.at<float>(i, j) = 0;
                        }
                    }
                }
                break;
            }
            default: {
                assert(false);
                break;
            }
        }

    }

}
