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

#include "manager_doc.h"
#include "base/base_def.h"
#include "base/base_markerpoint.h"

using namespace PspNsBase;

namespace PspNsManager {
    struct MANAGER_SHARED PspDocumentProcessConfig {
        bool mIsBackgroundImageSubtract;
        bool mIsFlatFieldCorrection;
        bool mIsFillUpMarkers;
        bool mIsThresholdWork;
        bool mIsDistortionCorrection;
        bool mIsAlignmentImages;
        bool mIsImageConvert;
        bool mIsFinalFiltering;

        PspDocumentProcessConfig() {
            mIsBackgroundImageSubtract = false;
            mIsFlatFieldCorrection = false;
            mIsFillUpMarkers = false;
            mIsThresholdWork = false;
            mIsDistortionCorrection = false;
            mIsAlignmentImages = false;
            mIsImageConvert = false;
            mIsFinalFiltering = false;
        }

        PspDocumentProcessConfig(const PspDocumentProcessConfig &config)
                : mIsBackgroundImageSubtract(config.mIsBackgroundImageSubtract),
                  mIsFlatFieldCorrection(config.mIsFlatFieldCorrection), mIsFillUpMarkers(config.mIsFillUpMarkers),
                  mIsThresholdWork(config.mIsThresholdWork), mIsDistortionCorrection(config.mIsDistortionCorrection),
                  mIsAlignmentImages(config.mIsAlignmentImages), mIsImageConvert(config.mIsImageConvert),
                  mIsFinalFiltering(config.mIsFinalFiltering) {

        }

        PspDocumentProcessConfig &operator=(const PspDocumentProcessConfig &config) {
            mIsBackgroundImageSubtract = config.mIsBackgroundImageSubtract;
            mIsFlatFieldCorrection = config.mIsFlatFieldCorrection;
            mIsFillUpMarkers = config.mIsFillUpMarkers;
            mIsThresholdWork = config.mIsThresholdWork;
            mIsDistortionCorrection = config.mIsDistortionCorrection;
            mIsAlignmentImages = config.mIsAlignmentImages;
            mIsImageConvert = config.mIsImageConvert;
            mIsFinalFiltering = config.mIsFinalFiltering;

            return *this;
        }
    };

    class MANAGER_SHARED PspDocumentSingleComponent : public PspDocument {
    public:
        PspDocumentSingleComponent();
        virtual ~PspDocumentSingleComponent();

        PspDocumentSingleComponent(const PspDocumentSingleComponent &document);
        PspDocumentSingleComponent &operator=(const PspDocumentSingleComponent &document);

        const QString &GetPathBackgroundImage() const;
        const QString &GetPathBackground2Image() const;
        const QString &GetPathWindOffImage() const;
        const QString &GetPathWindOnImage() const;
        const QString &GetPathWindOffMarker() const;
        const QString &GetPathWindOnMarker() const;
        const QString &GetDesc() const;
        const QString &GetTestFeaturesName() const;
        double GetTestFeaturesAttackAngle() const;
        double GetTestFeaturesSideSlipAngle() const;
        double GetTestFeaturesMach() const;
        double GetTestFeaturesWindOffTemperature() const;
        double GetTestFeaturesWindOnTemperature() const;
        double GetTestFeaturesFlowRatePressure() const;
        double GetTestFeaturesCompressedPressure() const;
        double GetTestFeaturesWindOffPressure() const;
        const QString &GetPathPaintCalibrationFile() const;
        const QString &GetPathLigthRationImg() const;
        const QString &GetPathPressureViewImg() const;
        const QString &GetPathPressureCoefficientImg() const;

        eUnitPressure GetTestFeaturesPressureUnit() const;
        eUnitTemperature GetTestFeaturesTemperatureUnit() const;
        double GetTestFeaturesStaticPressure() const;
        double GetTestFeaturesDynamicPressure() const;
        QString GetTestFeaturesTestId() const;
        QString GetTestFeaturesModelId() const;
        QString GetTestFeaturesTestPointId() const;
        double GetTestFeaturesAlpha() const;
        double GetTestFeaturesBeta() const;

        cv::Mat GetLightRatioImg();
        cv::Mat GetPressureImg();
        cv::Mat GetPressureCoefficientImg();

        cv::Mat GetBackgroundImg();
        cv::Mat GetBackground2Img();
        cv::Mat GetWindOnImg();
        cv::Mat GetWindOffImg();

        const std::vector<MarkerPoint> &GetWindOnMarkers() const;
        const std::vector<MarkerPoint> &GetWindOffMarkers() const;

        const PspDocumentProcessConfig &GetProcessConfig() const;

        double GetDeviationMaxX() const;
        double GetDeviationMaxY() const;
        double GetDeviationMaxDistance() const;
        double GetDeviationMeanX() const;
        double GetDeviationMeanY() const;
        double GetDeviationMeanDistance() const;

        void SetPathBackgroundImage(const QString &path);
        void SetPathBackground2Image(const QString &path);
        void SetPathWindOffImage(const QString &path);
        void SetPathWindOnImage(const QString &path);
        void SetPathWindOffMarker(const QString &path);
        void SetPathWindOnMarker(const QString &path);
        void SetDesc(const QString &desc);
        void SetTestFeaturesName(const QString &name);
        void SetTestFeaturesAttackAngle(double val);
        void SetTestFeaturesSideSlipAngle(double val);
        void SetTestFeaturesMach(double val);
        void SetTestFeaturesWindOffTemperature(double val);
        void SetTestFeaturesWindOnTemperature(double val);
        void SetTestFeaturesFlowRatePressure(double val);
        void SetTestFeaturesCompressedPressure(double val);
        void SetTestFeaturesWindOffPressure(double val);
        void SetPathPaintCalibrationFile(const QString &path);
        void SetPathLigthRationImg(const QString &path);
        void SetPathPressureViewImg(const QString &path);
        void SetPathPressureCoefficientImg(const QString &path);

        void SetTestFeaturesPressureUnit(eUnitPressure unit);
        void SetTestFeaturesTemperatureUnit(eUnitTemperature unit);
        void SetTestFeaturesStaticPressure(double val);
        void SetTestFeaturesDynamicPressure(double val);
        void SetTestFeaturesTestId(const QString &val);
        void SetTestFeaturesModelId(const QString &val);
        void SetTestFeaturesTestPointId(const QString &val);
        void SetTestFeaturesAlpha(double val);
        void SetTestFeaturesBeta(double val);

        void SetLightRatioImg(cv::Mat img);
        void SetPressureImg(cv::Mat img);
        void SetPressureCoefficientImg(cv::Mat img);

        void SetBackgroundImg(cv::Mat img);
        void SetBackground2Img(cv::Mat img);
        void SetWindOnImg(cv::Mat img);
        void SetWindOffImg(cv::Mat img);

        void SetWindOnMarkers(const std::vector<MarkerPoint> &markers);
        void SetWindOffMarkers(const std::vector<MarkerPoint> &markers);

        void SetProcessConfig(const PspDocumentProcessConfig &config);

        void SetDeviationMaxX(double val);
        void SetDeviationMaxY(double val);
        void SetDeviationMaxDistance(double val);
        void SetDeviationMeanX(double val);
        void SetDeviationMeanY(double val);
        void SetDeviationMeanDistance(double val);

        void ResetResult();
        void ResetConfig();

    private:
        QString mPathBackgroundImage;
        QString mPathBackground2Image;
        QString mPathWindOffImage;
        QString mPathWindOnImage;

        QString mPathWindOffMarker;
        QString mPathWindOnMarker;

        QString mDesc;

        QString mTestFeaturesName;
        double mTestFeaturesAttackAngle;
        double mTestFeaturesSideSlipAngle;
        double mTestFeaturesMach;
        double mTestFeaturesWindOffTemperature;
        double mTestFeaturesWindOnTemperature;
        double mTestFeaturesFlowRatePressure;
        double mTestFeaturesCompressedPressure;
        double mTestFeaturesWindOffPressure;

        eUnitPressure mTestFeaturesPressureUnit;
        eUnitTemperature mTestFeaturesTemperatureUnit;
        double mTestFeaturesStaticPressure;
        double mTestFeaturesDynamicPressure;
        QString mTestFeaturesTestId;
        QString mTestFeaturesModelId;
        QString mTestFeaturesTestPointId;
        double mTestFeaturesAlpha;
        double mTestFeaturesBeta;

        cv::Mat mLightRatioImg;
        cv::Mat mPressureImg;
        cv::Mat mPressureCoefficientImg;

        cv::Mat mBackgroundImg;
        cv::Mat mBackground2Img;
        cv::Mat mWindOnImg;
        cv::Mat mWindOffImg;

        std::vector<MarkerPoint> mWindOnMarkers;
        std::vector<MarkerPoint> mWindOffMarkers;

        QString mPathPaintCalibrationFile;
        QString mPathLigthRationImg;
        QString mPathPressureViewImg;
        QString mPathPressureCoefficientImg;

        PspDocumentProcessConfig mProcessConfig;

        double mDeviationMaxX;
        double mDeviationMaxY;
        double mDeviationMaxDistance;
        double mDeviationMeanX;
        double mDeviationMeanY;
        double mDeviationMeanDistance;
    };

    MANAGER_SHARED void to_json(Json &j, const PspDocumentSingleComponent &document);
    MANAGER_SHARED void from_json(const Json &j, PspDocumentSingleComponent &document);
}
