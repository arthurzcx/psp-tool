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
#include "algo/algo_match.h"
#include "algo/algo_marker_detector.h"
#include "algo/algo_threshold.h"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include "base/base_graphic_rect.h"
#include "base/base_system_param.h"
#include <QDebug>

using namespace PspNsBase;
using namespace cv;

namespace PspNsAlgo {
    PSP_ALGO_SHARED void SurfMatch(const Mat &src1, const Mat &src2, Mat &dst, int threshold1, int threshold2) {
        Ptr<cv::xfeatures2d::SURF> surfDetector = xfeatures2d::SurfFeatureDetector::create(threshold1);
        vector<KeyPoint> kp1, kp2;
        surfDetector->detect(src1, kp1);
        surfDetector->detect(src1, kp2);

        Mat desc1, desc2;
        surfDetector->compute(src1, kp1, desc1);
        surfDetector->compute(src1, kp2, desc2);

        FlannBasedMatcher flannBaseMatcher;
        vector<DMatch> dmatchs;
        flannBaseMatcher.match(desc1, desc2, dmatchs);

        drawMatches(src1, kp1, src2, kp2, dmatchs, dst, Scalar::all(255));
    }

    PSP_ALGO_SHARED void
    FindHomography(const vector<cv::Point2f> &kp1, const vector<cv::Point2f> &kp2, cv::Mat &homographyMatrix,
                   int methodType) {
        homographyMatrix = cv::findHomography(kp1, kp2, methodType);
    }

    PSP_ALGO_SHARED void Match(const cv::Mat &srcImg1, const cv::Mat &srcImg2, vector<cv::KeyPoint> &keyPoints1,
                               vector<cv::KeyPoint> &keyPoints2,
                               cv::Mat &homographyMatrix, cv::Mat &alignedDst) {
        auto descriptor = cv::SIFT::create();
        Mat description1;
        descriptor->compute(srcImg1, keyPoints1, description1);
        Mat description2;
        descriptor->compute(srcImg2, keyPoints2, description2);

        FlannBasedMatcher matcher;
        vector<DMatch> matches;
        matcher.match(description1, description2, matches);

        double max_dist = 0;
        double min_dist = 100;
        for (int i = 0; i < matches.size(); i++) {
            double dist = matches[i].distance;
            if (dist < min_dist)
                min_dist = dist;
            if (dist > max_dist)
                max_dist = dist;
        }

        vector<DMatch> good_matches;
        double dThreshold = 0.5;
        for (int i = 0; i < matches.size(); i++) {
            if (matches[i].distance < dThreshold * max_dist) {
                good_matches.push_back(matches[i]);
            }
        }


        vector<KeyPoint> R_keypoint01, R_keypoint02;
        for (int i = 0; i < good_matches.size(); i++) {
            R_keypoint01.push_back(keyPoints1[good_matches[i].queryIdx]);
            R_keypoint02.push_back(keyPoints2[good_matches[i].trainIdx]);
        }

        vector<Point2f> p01, p02;
        for (int i = 0; i < good_matches.size(); i++) {
            p01.push_back(R_keypoint01[i].pt);
            p02.push_back(R_keypoint02[i].pt);
        }

        vector<uchar> RansacStatus;
        homographyMatrix = findHomography(p01, p02, RansacStatus, cv::RANSAC);

        vector<KeyPoint> RR_keypoint01, RR_keypoint02;
        vector<DMatch> RR_matches;
        int index = 0;
        for (int i = 0; i < good_matches.size(); i++) {
            if (RansacStatus[i] != 0) {
                RR_keypoint01.push_back(R_keypoint01[i]);
                RR_keypoint02.push_back(R_keypoint02[i]);
                good_matches[i].queryIdx = index;
                good_matches[i].trainIdx = index;
                RR_matches.push_back(good_matches[i]);
                index++;
            }
        }

        drawMatches(srcImg1, RR_keypoint01, srcImg2, RR_keypoint02, RR_matches, alignedDst, Scalar(0, 255, 0),
                    Scalar::all(-1));
    }

    PSP_ALGO_SHARED bool FindHasSameRadiusEllipse(const vector<cv::RotatedRect> &ellipses, const MarkerPoint &mp,
                                                  cv::RotatedRect &resultEllipse) {
        if (ellipses.empty()) return false;

        double offset = 1.0E5;
        size_t indexSameRadius = 0;

        for (size_t index = 0; index < ellipses.size(); index++) {
            double ratioWidthHeight = ellipses[index].size.aspectRatio();
            if (ratioWidthHeight > 1.5 || ratioWidthHeight < 0.7) {
                continue;
            }

            double tmpOffset = fabs(ellipses[index].boundingRect().width - mp.getDetectHalfWidth() * 2);
            if (tmpOffset < offset) {
                offset = tmpOffset;
                indexSameRadius = index;
            }
        }

        resultEllipse = ellipses[indexSameRadius];
        return true;
    }


    PSP_ALGO_SHARED bool
    MatchTemplate(const cv::Mat &srcTemplate, const cv::Mat &srcMatch, const MarkerPoint &kpTemplate,
                  qreal searchRadius, MarkerPoint &kpMatch, int threshold) {
        if (srcTemplate.type() != srcMatch.type()) return false;
        GraphicRect tempRect = GraphicRect::CreateRectFromCenter(kpTemplate.getPoint(),
                                                                 kpTemplate.getDetectHalfWidth() + 8);
        cv::Mat temp(srcTemplate, cv::Rect(tempRect.GetLeft(), tempRect.GetTop(), tempRect.GetRect().width(),
                                           tempRect.GetRect().height()));

        GraphicRect roiRect = tempRect.ExpandByRaius(searchRadius, srcMatch.cols, srcMatch.rows);
        cv::Mat ROI(srcMatch, cv::Rect(roiRect.GetLeft(), roiRect.GetTop(), roiRect.GetRect().width(),
                                       roiRect.GetRect().height()));

        cv::Mat result(ROI.rows - temp.rows + 1, ROI.cols - temp.cols + 1, CV_32FC1);
        matchTemplate(ROI, temp, result, cv::TM_CCORR_NORMED);
        normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        double minVal, maxVal;
        cv::Point minLoc, matchLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &matchLoc, cv::Mat());

        if (SystemParam::GetInstance()->IsDebugMode()) {
            cv::rectangle(srcTemplate, cv::Rect(tempRect.GetLeft(), tempRect.GetTop(), tempRect.GetRect().width(),
                                                tempRect.GetRect().height()), cv::Scalar::all(0), 2, 8, 0);
            namedWindow("TemplateSrcImg", WINDOW_NORMAL);
            imshow("TemplateSrcImg", srcTemplate);
            cv::Mat img_display;
            srcMatch.copyTo(img_display);
            rectangle(srcMatch, cv::Rect(roiRect.GetLeft(), roiRect.GetTop(), roiRect.GetRect().width(),
                                         roiRect.GetRect().height()), cv::Scalar::all(100), 2, 8, 0);
            rectangle(srcMatch, cv::Point(matchLoc.x + roiRect.GetLeft(), matchLoc.y + roiRect.GetTop()),
                      cv::Point(matchLoc.x + roiRect.GetLeft() + temp.cols, matchLoc.y + roiRect.GetTop() + temp.rows),
                      cv::Scalar::all(255), 2, 8, 0);
            namedWindow("MatchedTemplates", WINDOW_NORMAL);
            imshow("MatchedTemplates", srcMatch);
        }
        cv::Mat matchedMat(ROI,
                           cv::Rect(matchLoc.x, matchLoc.y, tempRect.GetRect().width(), tempRect.GetRect().height()));

        vector<cv::RotatedRect> ellipses;
        vector<QPointF> points;

        double fscaleLargeMarker = 2.0;
        double fscaleSmallMarker = 10.0;
        PspNsAlgo::fitEllipse(matchedMat, ellipses, points, kpTemplate.getDetectHalfWidth(),
                              kpTemplate.getDetectHalfWidth() <= 3 ? fscaleSmallMarker : fscaleLargeMarker);

        if (ellipses.size() == points.size() && !points.empty()) {
            for (auto &ellipse: ellipses) {
                ellipse.center.x += matchLoc.x + roiRect.GetLeft();
                ellipse.center.y += matchLoc.y + roiRect.GetTop();
            }

            cv::RotatedRect resultRotatedRect;
            if (FindHasSameRadiusEllipse(ellipses, kpTemplate, resultRotatedRect)) {
                kpMatch.setX(resultRotatedRect.center.x);
                kpMatch.setY(resultRotatedRect.center.y);
                kpMatch.setDetectHalfWidth(resultRotatedRect.boundingRect().width / 2.0);
            }
        }

        return true;
    }
}
