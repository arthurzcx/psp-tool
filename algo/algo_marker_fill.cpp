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
#include "algo/algo_marker_fill.h"
#include "algo/algo_threshold.h"
#include "algo/algo_type.h"
#include "algo/algo_filter.h"

using namespace cv;

#include <QDebug>

namespace PspNsAlgo {
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

    void calLines(const vector<Point> &contour, vector<int> &val, vector<vector<int>> &lines, bool isHoriz) {
        if (contour.empty()) return;
        if (isHoriz) {
            for (auto point: contour) {
                val.push_back(point.y);
            }
            std::sort(val.begin(), val.end());
            auto end_unique = std::unique(val.begin(), val.end());
            val.erase(end_unique, val.end());

            lines.resize(val.size());
            for (size_t i = 0; i < val.size(); i++) {
                for (auto point: contour) {
                    if (point.y == val[i]) {
                        lines[i].push_back(point.x);
                    }
                }
                std::sort(lines[i].begin(), lines[i].end());
            }
        } else {
            for (auto point: contour) {
                val.push_back(point.x);
            }
            std::sort(val.begin(), val.end());
            auto end_unique = std::unique(val.begin(), val.end());
            val.erase(end_unique, val.end());

            lines.resize(val.size());
            for (size_t i = 0; i < val.size(); i++) {
                for (auto point: contour) {
                    if (point.x == val[i]) {
                        lines[i].push_back(point.y);
                    }
                }
                std::sort(lines[i].begin(), lines[i].end());
            }
        }
    }

    double calPixelNeighPixelAverage(cv::Mat &img, int x, int y, int number, bool isHoriz, bool isFront,
                                     float averageValRoiThreshold = 0.0) {
        if (number < 1) return -1.0;

        if (isHoriz) {
            int xbegin = max(0, (isFront ? x - number : x + 1));
            int xend = min((isFront ? x - 1 : x + number), img.cols - 1);

            if (xend <= xbegin) return -1.0;
            int delta = xend - xbegin + 1;

            double sum = 0.0;
            for (int ix = xbegin; ix <= xend; ix++) {
                if (img.depth() == CV_16U) {
                    if (img.at<ushort>(y, ix) < averageValRoiThreshold) {
                        delta--;
                        continue;
                    }
                    sum += img.at<ushort>(y, ix);
                } else if (img.depth() == CV_8U) {
                    if (img.at<uchar>(y, ix) < averageValRoiThreshold) {
                        delta--;
                        continue;
                    }

                    sum += img.at<uchar>(y, ix);
                } else if (img.depth() == CV_32F) {
                    if (img.at<float>(y, ix) < averageValRoiThreshold) {
                        delta--;
                        continue;
                    }

                    sum += img.at<float>(y, ix);
                } else {
                    assert(false);
                }
            }
            return sum / delta;
        } else {
            int ybegin = max(0, (isFront ? y - number : y + 1));
            int yend = min((isFront ? y - 1 : y + number), img.rows - 1);

            if (yend <= ybegin) return -1.0;
            int delta = yend - ybegin + 1;

            double sum = 0.0;
            for (int iy = ybegin; iy <= yend; iy++) {
                if (img.depth() == CV_16U) {
                    if (img.at<ushort>(iy, x) < averageValRoiThreshold) {
                        delta--;
                        continue;
                    }

                    sum += img.at<ushort>(iy, x);
                } else if (img.depth() == CV_8U) {
                    if (img.at<uchar>(iy, x) < averageValRoiThreshold) {
                        delta--;
                        continue;
                    }

                    sum += img.at<uchar>(iy, x);
                } else if (img.depth() == CV_32F) {
                    if (img.at<float>(iy, x) < averageValRoiThreshold) {
                        delta--;
                        continue;
                    }

                    sum += img.at<float>(iy, x);
                } else {
                    assert(false);
                }
            }
            return sum / delta;
        }

        return -1.0;
    }

    PSP_ALGO_SHARED void FillupMarkers(cv::Mat &img, const vector<MarkerPoint> &markers) {
        if (markers.empty()) return;

        vector<vector<cv::Point>> contours;
        vector<float> fAverage;
        vector<Vec4i> hierarchy;
        Mat temp;//create a new image
        img.copyTo(temp);
        temp = convertTo8UC1(temp);
        filterMedian(temp, temp, 3);

        for (auto marker: markers) {
            int extend = 3;
            vector<vector<cv::Point>> contoursTmp;
            int xleft = max(0, marker.x() - marker.getDetectHalfWidth() - extend);
            int width = min(marker.x() + marker.getDetectHalfWidth() + extend, img.cols - 1) - xleft;
            int ytop = max(0, marker.y() - marker.getDetectHalfWidth() - extend);
            int height = min(marker.y() + marker.getDetectHalfWidth() + extend, img.rows - 1) - ytop;

            cv::Rect roiRt = Rect(xleft, ytop, width, height);
            Mat roi = temp(roiRt);
            float fAverageVal = cv::mean(roi).val[0];

            double lowThreshold, highThreshold;
            calcAdapiveThreshold(roi, lowThreshold, highThreshold);
            Canny(roi, roi, lowThreshold, lowThreshold * 2);
            findContours(roi, contoursTmp, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

            for (auto &contour: contoursTmp) {
                for (auto &point: contour) {
                    point.x += roiRt.x;
                    point.y += roiRt.y;
                }

                fAverage.push_back(fAverageVal);
            }

            contours.insert(contours.end(), contoursTmp.begin(), contoursTmp.end());
        }

        assert(fAverage.size() == contours.size());

        int NeighborPixelNumber = 5;
        for (int counter = 0; counter < contours.size(); counter++) {
            auto contour = contours[counter];
            float averageValRatioThreshold = fAverage[counter] * 0.3;

            if (contour.size() < 3) continue;
            vector<int> yVal;
            vector<vector<int>> xLines;
            vector<int> xVal;
            vector<vector<int>> yLines;
            calLines(contour, yVal, xLines, true);
            calLines(contour, xVal, yLines, false);
            if (yVal.size() != xLines.size() || xVal.size() != yLines.size()) {
                assert(false);
            }

            for (int index = 0; index < yVal.size(); index++) {
                double pixelFront, pixelBack, pixelTop, pixelBottom;
                const vector<int> &xls = xLines[index];

                pixelFront = calPixelNeighPixelAverage(img, xls.front(), yVal[index], NeighborPixelNumber, true, true,
                                                       averageValRatioThreshold);
                pixelBack = calPixelNeighPixelAverage(img, xls.back(), yVal[index], NeighborPixelNumber, true, false,
                                                      averageValRatioThreshold);

                float deltaPixelHoriz = 0.0;
                if (xls.back() != xls.front()) {
                    deltaPixelHoriz = (pixelBack - pixelFront) / (xls.back() - xls.front());
                } else {
                    deltaPixelHoriz = 0.0;
                    pixelFront = (pixelFront + pixelBack) / 2.0;
                }

                for (int xIndex = xls.front(); xIndex <= xls.back(); xIndex++) {
                    float pixelHoriz = pixelFront + deltaPixelHoriz * (xIndex - xls.front());
                    float pixelVert = 0.0;
                    for (int xValIndex = 0; xValIndex < xVal.size(); xValIndex++) {
                        if (xVal[xValIndex] == xIndex) {
                            pixelTop = calPixelNeighPixelAverage(img, xVal[xValIndex], yLines[xValIndex].front(),
                                                                 NeighborPixelNumber, false, true,
                                                                 averageValRatioThreshold);
                            pixelBottom = calPixelNeighPixelAverage(img, xVal[xValIndex], yLines[xValIndex].back(),
                                                                    NeighborPixelNumber, false, false,
                                                                    averageValRatioThreshold);

                            if (yLines[xValIndex].back() != yLines[xValIndex].front()) {
                                double deltaPixelVert = (pixelBottom - pixelTop) /
                                                        (yLines[xValIndex].back() - yLines[xValIndex].front());
                                pixelVert = pixelTop + (yVal[index] - yLines[xValIndex].front()) * deltaPixelVert;
                            } else {
                                pixelVert = (pixelTop + pixelBottom) / 2.0;
                            }
                            break;
                        }
                    }

                    double pixelAverage = max(pixelHoriz, pixelVert);

                    if (img.depth() == CV_16U) {
                        img.at<ushort>(yVal[index], xIndex) = pixelAverage;
                    } else if (img.depth() == CV_8U) {
                        img.at<uchar>(yVal[index], xIndex) = pixelAverage;
                    } else if (img.depth() == CV_32F) {
                        img.at<float>(yVal[index], xIndex) = pixelAverage;
                    } else {
                        assert(false);
                    }
                }
            }

        }

    }
}
