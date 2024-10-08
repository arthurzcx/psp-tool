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
#include "algo/algo_basic_arithmatic.h"
#include "opencv2/core.hpp"

using namespace cv;

namespace PspNsAlgo {
    void add(const Mat &src1, const Mat &src2, Mat &dst) {

        cv::add(src1, src2, dst);
    }

    void subtract(const Mat &src1, const Mat &src2, Mat &dst) {

        cv::subtract(src1, src2, dst);
    }

    void multiply(const Mat &src1, const Mat &src2, Mat &dst) {

        cv::multiply(src1, src2, dst);
    }

    void divide(const Mat &src1, const Mat &src2, Mat &dst) {

        cv::divide(src1, src2, dst);
    }

    void divide32F(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst) {
        if (src1.depth() != CV_32F || src2.depth() != CV_32F) return;

        if (src1.rows != src2.rows || src1.cols != src2.cols) return;

        dst = cv::Mat(src1.rows, src1.cols, CV_32F);

        for (int i = 0; i < src1.rows; i++) {
            for (int j = 0; j < src1.cols; j++) {
                if (src1.at<float>(i, j) < FLT_EPSILON || src2.at<float>(i, j) < FLT_EPSILON) {
                    dst.at<float>(i, j) = 0.0;
                } else {
                    dst.at<float>(i, j) = src1.at<float>(i, j) / src2.at<float>(i, j);
                }
            }
        }
    }

    void scaleAdd(const Mat &src1, double alpha, const Mat &src2, Mat &dst) {

        cv::scaleAdd(src1, alpha, src2, dst);
    }

    void addWeighted(const Mat &src1, double alpha, const Mat &src2, double beta, double gamma, Mat &dst) {

        cv::addWeighted(src1, alpha, src2, beta, gamma, dst);
    }

    double getPixel(const cv::Mat &src, int x, int y) {
        if (x < 0 || x >= src.cols || y < 0 || y >= src.rows) {
            assert(false);
        }

        switch (src.depth()) {
            case CV_8U: {
                return src.at<uchar>(y, x);
            }
            case CV_16U: {
                return src.at<ushort>(y, x);
            }
            case CV_32F: {
                return src.at<float>(y, x);
            }
            default: {
                assert(false);
                return 0.0;
            }
        }
    }
}
