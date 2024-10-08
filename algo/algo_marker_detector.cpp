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
#include "algo/algo_marker_detector.h"
#include "algo/algo_threshold.h"
#include "algo/algo_type.h"
#include "base/base_trace.h"
#include <QDebug>

using namespace PspNsBase;
using namespace cv;

namespace PspNsAlgo {
    struct UniqueKeyPOINT__ : public binary_function<KeyPoint, KeyPoint, bool> {
        explicit UniqueKeyPOINT__(float dis = 5.0) : distance(dis) {}

        bool operator()(const KeyPoint &p0, const KeyPoint &p1) const {
            return (fabs(p0.pt.x - p1.pt.x) < distance && fabs(p0.pt.y - p1.pt.y) < distance);
        }

    private:
        float distance;
    };

    struct UniquePointF__ : public binary_function<QPointF, QPointF, bool> {
        explicit UniquePointF__(float dis = 5.0) : distance(dis) {}

        bool operator()(const QPointF &p0, const QPointF &p1) const {
            return (fabs(p0.x() - p1.x()) < distance && fabs(p0.y() - p1.y()) < distance);
        }


    private:
        float distance;
    };

    struct UniqueEllipse__ : public binary_function<cv::RotatedRect, cv::RotatedRect, bool> {
        explicit UniqueEllipse__(float dis = 5.0) : distance(dis) {}

        bool operator()(const RotatedRect &e0, const RotatedRect &e1) const {
            return (fabs(e0.center.x - e1.center.x) < distance && fabs(e0.center.y - e1.center.y) < distance);
        }


    private:
        float distance;
    };

    PSP_ALGO_SHARED void removeTooClosePoints(vector<KeyPoint> &points, float distance) {
        if (distance < 1.0 || points.size() <= 2) return;

        UniqueKeyPOINT__ uniqueKeyPoint(distance);
        points.erase(unique(points.begin(), points.end(), uniqueKeyPoint), points.end());
    }

    PSP_ALGO_SHARED void removeTooClosePoints(vector<QPointF> &points, float distance) {
        if (distance < 1.0 || points.size() < 2) return;

        UniquePointF__ uniquePoint(distance);
        points.erase(unique(points.begin(), points.end(), uniquePoint), points.end());
    }

    PSP_ALGO_SHARED void removeTooCloseEllipses(vector<cv::RotatedRect> &ellipses, float distance) {
        if (distance < 1.0 || ellipses.size() < 2.0) return;

        UniqueEllipse__ uniqueEllipse(distance);
        ellipses.erase(unique(ellipses.begin(), ellipses.end(), uniqueEllipse), ellipses.end());
    }

    PSP_ALGO_SHARED void removeInvalidPoints(vector<QPointF> &points, int xMax, int yMax) {
        for (auto iter = points.begin(); iter != points.end();) {
            if ((*iter).x() >= xMax || (*iter).y() >= yMax || (*iter).x() <= 0 || (*iter).y() <= 0) {
                iter = points.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    PSP_ALGO_SHARED void removeInvalidEllipses(vector<cv::RotatedRect> &ellipses, int xMax, int yMax) {
        if (ellipses.size() < 2) return;
        for (auto iter = ellipses.begin(); iter != ellipses.end();) {
            double ratioWidthHeight = (*iter).size.aspectRatio();
            if (ratioWidthHeight < 0.5 || ratioWidthHeight > 2.0) {
                iter = ellipses.erase(iter);
                continue;
            }

            if ((*iter).center.x >= xMax || (*iter).center.y >= yMax || (*iter).center.x <= 0 ||
                (*iter).center.y <= 0 ||
                (*iter).size.width <= 1 || (*iter).size.height <= 1
                || (*iter).size.width >= xMax || (*iter).size.height >= yMax) {
                iter = ellipses.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    PSP_ALGO_SHARED void fastDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = FastFeatureDetector::create(threshold);
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void agastDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {


        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = AgastFeatureDetector::create(threshold);
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);

    }

    PSP_ALGO_SHARED void akazeDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = AKAZE::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);

    }

    PSP_ALGO_SHARED void briskDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = BRISK::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void gfttDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = GFTTDetector::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void kazeDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = KAZE::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void mserDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = MSER::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void orbDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = ORB::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void
    simpleBlobDetector(const Mat &src, vector<QPointF> &points, int threshold, Mat &dst) {
        vector<KeyPoint> keyPoints;

        Ptr<FeatureDetector> fast = SimpleBlobDetector::create();
        fast->detect(src, keyPoints);

        removeTooClosePoints(keyPoints, 5.0);

        for (auto iter = keyPoints.begin(); iter != keyPoints.end(); ++iter) {
            points.push_back(QPointF((*iter).pt.x, (*iter).pt.y));
        }

        dst = src.clone();
        drawKeypoints(src, keyPoints, dst, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
    }

    PSP_ALGO_SHARED void fitEllipse(const Mat &src, vector<RotatedRect> &ellipses, vector<QPointF> &points,
                                    unsigned int radiusMarker, double fscale) {

        Mat gray, ThreshGray;

        gray = convertTo8UC1(src);

        // subpixel interpolation to improve fitEllipse accuracy
        resize(gray, gray, cv::Size(), fscale, fscale, cv::INTER_CUBIC);

        // filter the image
        if (radiusMarker > 2) {
            // should not filter when the key point is too small
            blur(gray, gray, Size(3 + fscale - 1, 3 + fscale - 1));
        }

        // If the marker points are very small in size, improve the image contrast
        if (radiusMarker < 3 && fscale > 3.0) {
            // equalizeHist is suitable for most of cases, new image enhancement algorithms need to be proposed
            equalizeHist(gray, gray);//When marker is small, the contrast is usually strong enough

            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(gray, &minVal, &maxVal, &minLoc, &maxLoc);
            points.push_back(QPointF((double) minLoc.x / fscale, (double) minLoc.y / fscale));
            return;
        }

        // calculate the threshold value for edge detection
        double lowThreshold, highThreshold;
        calcOstuThreshold(gray, lowThreshold, highThreshold);
        double threshold = lowThreshold;

        // edge detection and get the contour
        Canny(gray, gray, threshold, threshold * 2);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // ellipse detection
        ellipses.clear();
        points.clear();
        for (size_t i = 0; i < contours.size(); i++) {
            if (contours[i].size() > 5 && contours[i].size() < 300 * fscale) {
                RotatedRect rotatedRect = cv::fitEllipse(contours[i]);
                ellipses.push_back(rotatedRect);
            }
        }
        removeTooCloseEllipses(ellipses, 5);
        removeInvalidEllipses(ellipses, src.cols * fscale, src.rows * fscale);

        // save the ellipse info
        for (auto &ellipse: ellipses) {
            ellipse.center.x /= fscale;
            ellipse.center.y /= fscale;
            ellipse.size.width /= fscale;
            ellipse.size.height /= fscale;
            points.push_back(QPointF(ellipse.center.x, ellipse.center.y));
        }
    }
}
