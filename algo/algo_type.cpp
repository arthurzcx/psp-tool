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
#include "algo/algo_type.h"
#include <vector>

namespace PspNsAlgo {
    cv::Mat convertTo8UC1(cv::Mat inputMat) {
        if (inputMat.type() == CV_8UC1) {
            return inputMat;
        }

        cv::Mat tmp = inputMat;
        if (inputMat.channels() != 1) {
            std::vector<cv::Mat> channels;
            channels.resize(tmp.channels());

            cv::split(tmp, &channels[0]);
            tmp = channels[0];

            if (inputMat.depth() == CV_8U) {
                return tmp;
            }
        }

        double minVal, maxVal;
        cv::minMaxLoc(tmp, &minVal, &maxVal);

        if (fabs(maxVal - minVal) > FLT_EPSILON) {
            tmp.convertTo(tmp, CV_8UC1, 255.0 / (maxVal - minVal), -255.0 * minVal / (maxVal - minVal));
        } else {
            tmp.convertTo(tmp, CV_8UC1, 0, 255);
        }
        return tmp;
    }

    cv::Mat convertTo16UC1(cv::Mat inputMat) {
        if (inputMat.type() == CV_16UC1) {
            return inputMat;
        }

        cv::Mat tmp = inputMat;

        if (inputMat.channels() != 1) {
            std::vector<cv::Mat> channels;
            channels.resize(tmp.channels());

            cv::split(tmp, &channels[0]);
            tmp = channels[0];

            if (inputMat.depth() == CV_16U) {
                return tmp;
            }
        }

        double minVal, maxVal;
        cv::minMaxLoc(tmp, &minVal, &maxVal);

        if (fabs(maxVal - minVal) > FLT_EPSILON) {
            tmp.convertTo(tmp, CV_16UC1, 65535.0 / (maxVal - minVal), -65535.0 * minVal / (maxVal - minVal));
        } else {
            tmp.convertTo(tmp, CV_16UC1, 0, 65535);
        }

        return tmp;
    }
}
