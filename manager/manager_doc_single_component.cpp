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
#include "manager/manager_doc_single_component.h"
#include "io/io.h"
#include "base/base_trace.h"

namespace PspNsManager {
    PspDocumentSingleComponent::PspDocumentSingleComponent()
            : mPathBackgroundImage(""), mPathBackground2Image(""), mPathWindOffImage(""), mPathWindOnImage(""),
              mPathWindOffMarker(""), mPathWindOnMarker(""), mDesc(""), mTestFeaturesName(""),
              mTestFeaturesAttackAngle(0.0), mTestFeaturesSideSlipAngle(0.0), mTestFeaturesMach(0.0),
              mTestFeaturesWindOffTemperature(0.0), mTestFeaturesWindOnTemperature(0.0),
              mTestFeaturesFlowRatePressure(0.0), mTestFeaturesCompressedPressure(0.0),
              mTestFeaturesWindOffPressure(0.0), mTestFeaturesPressureUnit(eUnitPressure::ePa),
              mTestFeaturesTemperatureUnit(eUnitTemperature::eC), mTestFeaturesStaticPressure(0.0),
              mTestFeaturesDynamicPressure(0.0), mTestFeaturesTestId(""), mTestFeaturesModelId(""),
              mTestFeaturesTestPointId(""), mTestFeaturesAlpha(0.0), mTestFeaturesBeta(0.0),
              mPathPaintCalibrationFile(""), mPathLigthRationImg(""), mPathPressureViewImg(""),
              mPathPressureCoefficientImg(""), mDeviationMaxX(0.0), mDeviationMaxY(0.0), mDeviationMaxDistance(0.0),
              mDeviationMeanX(0.0), mDeviationMeanY(0.0), mDeviationMeanDistance(0.0) {

    }
    PspDocumentSingleComponent::~PspDocumentSingleComponent() {

    }

    PspDocumentSingleComponent::PspDocumentSingleComponent(const PspDocumentSingleComponent &document)
            : mPathBackgroundImage(document.mPathBackgroundImage),
              mPathBackground2Image(document.mPathBackground2Image), mPathWindOffImage(document.mPathWindOffImage),
              mPathWindOnImage(document.mPathWindOnImage), mPathWindOffMarker(document.mPathWindOffMarker),
              mPathWindOnMarker(document.mPathWindOnMarker), mDesc(document.mDesc),
              mTestFeaturesName(document.mTestFeaturesName),
              mTestFeaturesAttackAngle(document.mTestFeaturesAttackAngle),
              mTestFeaturesSideSlipAngle(document.mTestFeaturesSideSlipAngle),
              mTestFeaturesMach(document.mTestFeaturesMach),
              mTestFeaturesWindOffTemperature(document.mTestFeaturesWindOffTemperature),
              mTestFeaturesWindOnTemperature(document.mTestFeaturesWindOnTemperature),
              mTestFeaturesFlowRatePressure(document.mTestFeaturesFlowRatePressure),
              mTestFeaturesCompressedPressure(document.mTestFeaturesCompressedPressure),
              mTestFeaturesWindOffPressure(document.mTestFeaturesWindOffPressure),
              mTestFeaturesPressureUnit(document.mTestFeaturesPressureUnit),
              mTestFeaturesTemperatureUnit(document.mTestFeaturesTemperatureUnit),
              mTestFeaturesStaticPressure(document.mTestFeaturesStaticPressure),
              mTestFeaturesDynamicPressure(document.mTestFeaturesDynamicPressure),
              mTestFeaturesTestId(document.mTestFeaturesTestId), mTestFeaturesModelId(document.mTestFeaturesModelId),
              mTestFeaturesTestPointId(document.mTestFeaturesTestPointId),
              mTestFeaturesAlpha(document.mTestFeaturesAlpha), mTestFeaturesBeta(document.mTestFeaturesBeta),
              mLightRatioImg(document.mLightRatioImg), mPressureImg(document.mPressureImg),
              mPressureCoefficientImg(document.mPressureCoefficientImg), mBackgroundImg(document.mBackgroundImg),
              mBackground2Img(document.mBackground2Img), mWindOnImg(document.mWindOnImg),
              mWindOffImg(document.mWindOffImg), mWindOnMarkers(document.mWindOnMarkers),
              mWindOffMarkers(document.mWindOffMarkers), mPathPaintCalibrationFile(document.mPathPaintCalibrationFile),
              mPathLigthRationImg(document.mPathLigthRationImg), mPathPressureViewImg(document.mPathPressureViewImg),
              mPathPressureCoefficientImg(document.mPathPressureCoefficientImg),
              mProcessConfig(document.mProcessConfig), mDeviationMaxX(document.mDeviationMaxX),
              mDeviationMaxY(document.mDeviationMaxY), mDeviationMaxDistance(document.mDeviationMaxDistance),
              mDeviationMeanX(document.mDeviationMeanX), mDeviationMeanY(document.mDeviationMeanY),
              mDeviationMeanDistance(document.mDeviationMeanDistance) {

    }
    PspDocumentSingleComponent &PspDocumentSingleComponent::operator=(const PspDocumentSingleComponent &document) {
        mPathBackgroundImage = document.mPathBackgroundImage;
        mPathBackground2Image = document.mPathBackground2Image;
        mPathWindOnImage = document.mPathWindOnImage;
        mPathWindOffImage = document.mPathWindOffImage;

        mLightRatioImg = document.mLightRatioImg;
        mPressureImg = document.mPressureImg;
        mPressureCoefficientImg = document.mPressureCoefficientImg;

        mPathWindOnMarker = document.mPathWindOnMarker;
        mPathWindOffMarker = document.mPathWindOffMarker;
        mDesc = document.mDesc;
        mTestFeaturesName = document.mTestFeaturesName;
        mTestFeaturesAttackAngle = document.mTestFeaturesAttackAngle;
        mTestFeaturesSideSlipAngle = document.mTestFeaturesSideSlipAngle;
        mTestFeaturesMach = document.mTestFeaturesMach;
        mTestFeaturesWindOffTemperature = document.mTestFeaturesWindOffTemperature;
        mTestFeaturesWindOnTemperature = document.mTestFeaturesWindOnTemperature;
        mTestFeaturesFlowRatePressure = document.mTestFeaturesFlowRatePressure;
        mTestFeaturesCompressedPressure = document.mTestFeaturesCompressedPressure;
        mTestFeaturesWindOffPressure = document.mTestFeaturesWindOffPressure;

        mTestFeaturesPressureUnit = document.mTestFeaturesPressureUnit;
        mTestFeaturesTemperatureUnit = document.mTestFeaturesTemperatureUnit;
        mTestFeaturesStaticPressure = document.mTestFeaturesStaticPressure;
        mTestFeaturesDynamicPressure = document.mTestFeaturesDynamicPressure;
        mTestFeaturesTestId = document.mTestFeaturesTestId;
        mTestFeaturesModelId = document.mTestFeaturesModelId;
        mTestFeaturesTestPointId = document.mTestFeaturesTestPointId;
        mTestFeaturesAlpha = document.mTestFeaturesAlpha;
        mTestFeaturesBeta = document.mTestFeaturesBeta;

        mPathPaintCalibrationFile = document.mPathPaintCalibrationFile;
        mPathLigthRationImg = document.mPathLigthRationImg;
        mPathPressureViewImg = document.mPathPressureViewImg;
        mPathPressureCoefficientImg = document.mPathPressureCoefficientImg;
        mProcessConfig = document.mProcessConfig;

        mBackgroundImg = document.mBackgroundImg;
        mBackground2Img = document.mBackground2Img;
        mWindOnImg = document.mWindOnImg;
        mWindOffImg = document.mWindOffImg;
        mWindOnMarkers = document.mWindOnMarkers;
        mWindOffMarkers = document.mWindOffMarkers;

        mDeviationMaxX = document.mDeviationMaxX;
        mDeviationMaxY = document.mDeviationMaxY;
        mDeviationMaxDistance = document.mDeviationMaxDistance;
        mDeviationMeanX = document.mDeviationMeanX;
        mDeviationMeanY = document.mDeviationMeanY;
        mDeviationMeanDistance = document.mDeviationMeanDistance;

        return *this;
    }

    const QString &PspDocumentSingleComponent::GetPathBackgroundImage() const {
        return mPathBackgroundImage;
    }

    const QString &PspDocumentSingleComponent::GetPathBackground2Image() const {
        return mPathBackground2Image;
    }

    const QString &PspDocumentSingleComponent::GetPathWindOffImage() const {
        return mPathWindOffImage;
    }

    const QString &PspDocumentSingleComponent::GetPathWindOnImage() const {
        return mPathWindOnImage;
    }

    const QString &PspDocumentSingleComponent::GetPathWindOffMarker() const {
        return mPathWindOffMarker;
    }

    const QString &PspDocumentSingleComponent::GetPathWindOnMarker() const {
        return mPathWindOnMarker;
    }

    const QString &PspDocumentSingleComponent::GetDesc() const {
        return mDesc;
    }

    const QString &PspDocumentSingleComponent::GetTestFeaturesName() const {
        return mTestFeaturesName;
    }

    double PspDocumentSingleComponent::GetTestFeaturesAttackAngle() const {
        return mTestFeaturesAttackAngle;
    }

    double PspDocumentSingleComponent::GetTestFeaturesSideSlipAngle() const {
        return mTestFeaturesSideSlipAngle;
    }

    double PspDocumentSingleComponent::GetTestFeaturesMach() const {
        return mTestFeaturesMach;
    }

    double PspDocumentSingleComponent::GetTestFeaturesWindOffTemperature() const {
        return mTestFeaturesWindOffTemperature;
    }

    double PspDocumentSingleComponent::GetTestFeaturesWindOnTemperature() const {
        return mTestFeaturesWindOnTemperature;
    }

    double PspDocumentSingleComponent::GetTestFeaturesFlowRatePressure() const {
        return mTestFeaturesFlowRatePressure;
    }

    double PspDocumentSingleComponent::GetTestFeaturesCompressedPressure() const {
        return mTestFeaturesCompressedPressure;
    }

    double PspDocumentSingleComponent::GetTestFeaturesWindOffPressure() const {
        return mTestFeaturesWindOffPressure;
    }

    const QString &PspDocumentSingleComponent::GetPathPaintCalibrationFile() const {
        return mPathPaintCalibrationFile;
    }

    const QString &PspDocumentSingleComponent::GetPathLigthRationImg() const {
        return mPathLigthRationImg;
    }

    const QString &PspDocumentSingleComponent::GetPathPressureViewImg() const {
        return mPathPressureViewImg;
    }

    const QString &PspDocumentSingleComponent::GetPathPressureCoefficientImg() const {
        return mPathPressureCoefficientImg;
    }

    eUnitPressure PspDocumentSingleComponent::GetTestFeaturesPressureUnit() const {
        return mTestFeaturesPressureUnit;
    }

    eUnitTemperature PspDocumentSingleComponent::GetTestFeaturesTemperatureUnit() const {
        return mTestFeaturesTemperatureUnit;
    }

    double PspDocumentSingleComponent::GetTestFeaturesStaticPressure() const {
        return mTestFeaturesStaticPressure;
    }

    double PspDocumentSingleComponent::GetTestFeaturesDynamicPressure() const {
        return mTestFeaturesDynamicPressure;
    }

    QString PspDocumentSingleComponent::GetTestFeaturesTestId() const {
        return mTestFeaturesTestId;
    }

    QString PspDocumentSingleComponent::GetTestFeaturesModelId() const {
        return mTestFeaturesModelId;
    }

    QString PspDocumentSingleComponent::GetTestFeaturesTestPointId() const {
        return mTestFeaturesTestPointId;
    }

    double PspDocumentSingleComponent::GetTestFeaturesAlpha() const {
        return mTestFeaturesAlpha;
    }

    double PspDocumentSingleComponent::GetTestFeaturesBeta() const {
        return mTestFeaturesBeta;
    }

    cv::Mat PspDocumentSingleComponent::GetLightRatioImg() {
        return mLightRatioImg;
    }

    cv::Mat PspDocumentSingleComponent::GetPressureImg() {
        return mPressureImg;
    }

    cv::Mat PspDocumentSingleComponent::GetPressureCoefficientImg() {
        return mPressureCoefficientImg;
    }

    cv::Mat PspDocumentSingleComponent::GetBackgroundImg() {
        return mBackgroundImg;
    }

    cv::Mat PspDocumentSingleComponent::GetBackground2Img() {
        return mBackground2Img;
    }

    cv::Mat PspDocumentSingleComponent::GetWindOnImg() {
        return mWindOnImg;
    }

    cv::Mat PspDocumentSingleComponent::GetWindOffImg() {
        return mWindOffImg;
    }

    const std::vector<MarkerPoint> &PspDocumentSingleComponent::GetWindOnMarkers() const {
        return mWindOnMarkers;
    }

    const std::vector<MarkerPoint> &PspDocumentSingleComponent::GetWindOffMarkers() const {
        return mWindOffMarkers;
    }

    const PspDocumentProcessConfig &PspDocumentSingleComponent::GetProcessConfig() const {
        return mProcessConfig;
    }

    double PspDocumentSingleComponent::GetDeviationMaxX() const {
        return mDeviationMaxX;
    }

    double PspDocumentSingleComponent::GetDeviationMaxY() const {
        return mDeviationMaxY;
    }

    double PspDocumentSingleComponent::GetDeviationMaxDistance() const {
        return mDeviationMaxDistance;
    }

    double PspDocumentSingleComponent::GetDeviationMeanX() const {
        return mDeviationMeanX;
    }

    double PspDocumentSingleComponent::GetDeviationMeanY() const {
        return mDeviationMeanY;
    }

    double PspDocumentSingleComponent::GetDeviationMeanDistance() const {
        return mDeviationMeanDistance;
    }

    void PspDocumentSingleComponent::SetPathBackgroundImage(const QString &path) {
        mPathBackgroundImage = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathBackgroundImage.toStdString())) {
            mBackgroundImg = io.getSrcImg();
        }
    }

    void PspDocumentSingleComponent::SetPathBackground2Image(const QString &path) {
        mPathBackground2Image = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathBackground2Image.toStdString())) {
            mBackground2Img = io.getSrcImg();
        }
    }

    void PspDocumentSingleComponent::SetPathWindOffImage(const QString &path) {
        mPathWindOffImage = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathWindOffImage.toStdString())) {
            mWindOffImg = io.getSrcImg();
        }
    }

    void PspDocumentSingleComponent::SetPathWindOnImage(const QString &path) {
        mPathWindOnImage = path;

        PspNsImgIo::Img_io io;
        if (io.readImg(mPathWindOnImage.toStdString())) {
            mWindOnImg = io.getSrcImg();
        }
    }

    void PspDocumentSingleComponent::SetPathWindOffMarker(const QString &path) {
        mPathWindOffMarker = path;
    }

    void PspDocumentSingleComponent::SetDesc(const QString &desc) {
        mDesc = desc;
    }

    void PspDocumentSingleComponent::SetTestFeaturesName(const QString &name) {
        mTestFeaturesName = name;
    }

    void PspDocumentSingleComponent::SetTestFeaturesAttackAngle(double val) {
        mTestFeaturesAttackAngle = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesSideSlipAngle(double val) {
        mTestFeaturesSideSlipAngle = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesMach(double val) {
        mTestFeaturesMach = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesWindOffTemperature(double val) {
        mTestFeaturesWindOffTemperature = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesWindOnTemperature(double val) {
        mTestFeaturesWindOnTemperature = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesFlowRatePressure(double val) {
        mTestFeaturesFlowRatePressure = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesCompressedPressure(double val) {
        mTestFeaturesCompressedPressure = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesWindOffPressure(double val) {
        mTestFeaturesWindOffPressure = val;
    }

    void PspDocumentSingleComponent::SetPathPaintCalibrationFile(const QString &path) {
        mPathPaintCalibrationFile = path;
    }

    void PspDocumentSingleComponent::SetPathLigthRationImg(const QString &path) {
        mPathLigthRationImg = path;
    }

    void PspDocumentSingleComponent::SetPathPressureViewImg(const QString &path) {
        mPathPressureViewImg = path;
    }

    void PspDocumentSingleComponent::SetPathPressureCoefficientImg(const QString &path) {
        mPathPressureCoefficientImg = path;
    }

    void PspDocumentSingleComponent::SetTestFeaturesPressureUnit(eUnitPressure unit) {
        mTestFeaturesPressureUnit = unit;
    }

    void PspDocumentSingleComponent::SetTestFeaturesTemperatureUnit(eUnitTemperature unit) {
        mTestFeaturesTemperatureUnit = unit;
    }

    void PspDocumentSingleComponent::SetTestFeaturesStaticPressure(double val) {
        mTestFeaturesStaticPressure = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesDynamicPressure(double val) {
        mTestFeaturesDynamicPressure = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesTestId(const QString &val) {
        mTestFeaturesTestId = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesModelId(const QString &val) {
        mTestFeaturesModelId = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesTestPointId(const QString &val) {
        mTestFeaturesTestPointId = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesAlpha(double val) {
        mTestFeaturesAlpha = val;
    }

    void PspDocumentSingleComponent::SetTestFeaturesBeta(double val) {
        mTestFeaturesBeta = val;
    }

    void PspDocumentSingleComponent::SetPathWindOnMarker(const QString &path) {
        mPathWindOnMarker = path;
    }

    void PspDocumentSingleComponent::SetLightRatioImg(cv::Mat img) {
        mLightRatioImg = img;
    }

    void PspDocumentSingleComponent::SetPressureImg(cv::Mat img) {
        mPressureImg = img;
    }

    void PspDocumentSingleComponent::SetPressureCoefficientImg(cv::Mat img) {
        mPressureCoefficientImg = img;
    }

    void PspDocumentSingleComponent::SetBackgroundImg(cv::Mat img) {
        mBackgroundImg = img;
    }

    void PspDocumentSingleComponent::SetBackground2Img(cv::Mat img) {
        mBackground2Img = img;
    }

    void PspDocumentSingleComponent::SetWindOnImg(cv::Mat img) {
        mWindOnImg = img;
    }

    void PspDocumentSingleComponent::SetWindOffImg(cv::Mat img) {
        mWindOffImg = img;
    }

    void PspDocumentSingleComponent::SetWindOnMarkers(const std::vector<MarkerPoint> &markers) {
        mWindOnMarkers = markers;
    }

    void PspDocumentSingleComponent::SetWindOffMarkers(const std::vector<MarkerPoint> &markers) {
        mWindOffMarkers = markers;
    }

    void PspDocumentSingleComponent::SetProcessConfig(const PspDocumentProcessConfig &config) {
        mProcessConfig = config;
    }

    void PspDocumentSingleComponent::SetDeviationMaxX(double val) {
        mDeviationMaxX = val;
    }

    void PspDocumentSingleComponent::SetDeviationMaxY(double val) {
        mDeviationMaxY = val;
    }

    void PspDocumentSingleComponent::SetDeviationMaxDistance(double val) {
        mDeviationMaxDistance = val;
    }

    void PspDocumentSingleComponent::SetDeviationMeanX(double val) {
        mDeviationMeanX = val;
    }

    void PspDocumentSingleComponent::SetDeviationMeanY(double val) {
        mDeviationMeanY = val;
    }

    void PspDocumentSingleComponent::SetDeviationMeanDistance(double val) {
        mDeviationMeanDistance = val;
    }

    void PspDocumentSingleComponent::ResetResult() {
        mPathLigthRationImg = "";
        mPathPressureViewImg = "";
        mPathPressureCoefficientImg = "";

        mLightRatioImg = cv::Mat();
        mPressureImg = cv::Mat();
        mPressureCoefficientImg = cv::Mat();
    }

    void PspDocumentSingleComponent::ResetConfig() {
        mProcessConfig = PspDocumentProcessConfig();
    }

    void to_json(Json &j, const PspDocumentProcessConfig &config) {
        j["IsBackgroundImageSubtract"] = config.mIsBackgroundImageSubtract;
        j["IsFlatFieldCorrection"] = config.mIsFlatFieldCorrection;
        j["IsFillUpMarkers"] = config.mIsFillUpMarkers;
        j["IsThresholdWork"] = config.mIsThresholdWork;
        j["IsDistortionCorrection"] = config.mIsDistortionCorrection;
        j["IsAlignmentImages"] = config.mIsAlignmentImages;
        j["IsImageConvert"] = config.mIsImageConvert;
        j["IsFinalFiltering"] = config.mIsFinalFiltering;
    }

    void from_json(const Json &j, PspDocumentProcessConfig &config) {
        config.mIsBackgroundImageSubtract = j["IsBackgroundImageSubtract"];
        config.mIsFlatFieldCorrection = j["IsFlatFieldCorrection"];
        config.mIsFillUpMarkers = j["IsFillUpMarkers"];
        config.mIsThresholdWork = j["IsThresholdWork"];
        config.mIsDistortionCorrection = j["IsDistortionCorrection"];
        config.mIsAlignmentImages = j["IsAlignmentImages"];
        config.mIsImageConvert = j["IsImageConvert"];
        config.mIsFinalFiltering = j["IsFinalFiltering"];
    }

    void to_json(Json &j, const PspDocumentSingleComponent &document) {
        j["cpuid"] = PspNsBase::PspTrace::getCpuId().toStdString();

        j["type"] = "Single component document";

        j["PathBackgroundImage"] = document.GetPathBackgroundImage().toStdString();
        j["PathBackground2Image"] = document.GetPathBackground2Image().toStdString();
        j["PathWindOffImage"] = document.GetPathWindOffImage().toStdString();
        j["PathWindOnImage"] = document.GetPathWindOnImage().toStdString();

        j["PathWindOffMarker"] = document.GetPathWindOffMarker().toStdString();
        j["PathWindOnMarker"] = document.GetPathWindOnMarker().toStdString();

        j["PathPaintCalibrationFile"] = document.GetPathPaintCalibrationFile().toStdString();
        j["PathPressureCoefficientImg"] = document.GetPathPressureCoefficientImg().toStdString();

        j["PathLightRatioImg"] = document.GetPathLigthRationImg().toStdString();
        j["PathPressureViewImg"] = document.GetPathPressureViewImg().toStdString();

        j["Desc"] = document.GetDesc().toStdString();

        j["TestFeatures"]["Mach"] = document.GetTestFeaturesMach();
        j["TestFeatures"]["WindOffTemperature"] = document.GetTestFeaturesWindOffTemperature();
        j["TestFeatures"]["WindOnTemperature"] = document.GetTestFeaturesWindOnTemperature();
        j["TestFeatures"]["WindOffPressure"] = document.GetTestFeaturesWindOffPressure();

        j["TestFeatures"]["PressureUnit"] = (int) document.GetTestFeaturesPressureUnit();
        j["TestFeatures"]["TemperatureUnit"] = (int) document.GetTestFeaturesTemperatureUnit();
        j["TestFeatures"]["StaticPressure"] = document.GetTestFeaturesStaticPressure();
        j["TestFeatures"]["DynamicPressure"] = document.GetTestFeaturesDynamicPressure();
        j["TestFeatures"]["TestId"] = document.GetTestFeaturesTestId().toStdString();
        j["TestFeatures"]["ModelId"] = document.GetTestFeaturesModelId().toStdString();
        j["TestFeatures"]["TestPointId"] = document.GetTestFeaturesTestPointId().toStdString();
        j["TestFeatures"]["Alpha"] = document.GetTestFeaturesAlpha();
        j["TestFeatures"]["Beta"] = document.GetTestFeaturesBeta();

        to_json(j["ProcessConfig"], document.GetProcessConfig());

        if (!document.GetWindOnMarkers().empty()) {
            for (auto marker: document.GetWindOnMarkers()) {
                Json jsonMarker;
                to_json(jsonMarker, marker);
                j["markersWindOn"].push_back(jsonMarker);
            }
        }

        if (!document.GetWindOffMarkers().empty()) {
            for (auto marker: document.GetWindOffMarkers()) {
                Json jsonMarker;
                to_json(jsonMarker, marker);
                j["markersWindOff"].push_back(jsonMarker);
            }
        }

        j["DeviationMaxX"] = document.GetDeviationMaxX();
        j["DeviationMaxY"] = document.GetDeviationMaxY();
        j["DeviationMaxDistance"] = document.GetDeviationMaxDistance();
        j["DeviationMeanX"] = document.GetDeviationMeanX();
        j["DeviationMeanY"] = document.GetDeviationMeanY();
        j["DeviationMeanDistance"] = document.GetDeviationMeanDistance();
    }

    void from_json(const Json &j, PspDocumentSingleComponent &document) {
        document.SetPathBackgroundImage(QString::fromStdString(j["PathBackgroundImage"]));
        if (j.contains("PathBackground2Image")) {
            document.SetPathBackground2Image(QString::fromStdString(j["PathBackground2Image"]));
        }
        document.SetPathWindOffImage(QString::fromStdString(j["PathWindOffImage"]));
        document.SetPathWindOnImage(QString::fromStdString(j["PathWindOnImage"]));

        document.SetPathWindOffMarker(QString::fromStdString(j["PathWindOffMarker"]));
        document.SetPathWindOnMarker(QString::fromStdString(j["PathWindOnMarker"]));

        if (j.contains("PathPaintCalibrationFile")) {
            document.SetPathPaintCalibrationFile(QString::fromStdString(j["PathPaintCalibrationFile"]));
        }

        if (j.contains("PathLightRatioImg")) {
            document.SetPathLigthRationImg(QString::fromStdString(j["PathLightRatioImg"]));
        }

        if (j.contains("PathPressureViewImg")) {
            document.SetPathPressureViewImg(QString::fromStdString(j["PathPressureViewImg"]));
        }

        if (j.contains("PathPressureCoefficientImg")) {
            document.SetPathPressureCoefficientImg(QString::fromStdString(j["PathPressureCoefficientImg"]));
        }

        if (j.contains("Desc")) {
            document.SetDesc(QString::fromStdString(j["Desc"]));
        }

//        document.SetTestFeaturesName(QString::fromStdString(j["TestFeatures"]["Name"]));
//        document.SetTestFeaturesAttackAngle(j["TestFeatures"]["AttackAngle"]);
//        document.SetTestFeaturesSideSlipAngle(j["TestFeatures"]["SideSlipAngle"]);
        document.SetTestFeaturesMach(j["TestFeatures"]["Mach"]);
        document.SetTestFeaturesWindOffTemperature(j["TestFeatures"]["WindOffTemperature"]);
        document.SetTestFeaturesWindOnTemperature(j["TestFeatures"]["WindOnTemperature"]);
//        document.SetTestFeaturesFlowRatePressure(j["TestFeatures"]["FlowRatePressure"]);
//        document.SetTestFeaturesCompressedPressure(j["TestFeatures"]["CompressedPressure"]);
        document.SetTestFeaturesWindOffPressure(j["TestFeatures"]["WindOffPressure"]);

        document.SetTestFeaturesPressureUnit((eUnitPressure) j["TestFeatures"]["PressureUnit"]);
        document.SetTestFeaturesTemperatureUnit((eUnitTemperature) j["TestFeatures"]["TemperatureUnit"]);
        document.SetTestFeaturesStaticPressure(j["TestFeatures"]["StaticPressure"]);
        document.SetTestFeaturesDynamicPressure(j["TestFeatures"]["DynamicPressure"]);
        document.SetTestFeaturesTestId(QString::fromStdString(j["TestFeatures"]["TestId"]));
        document.SetTestFeaturesModelId(QString::fromStdString(j["TestFeatures"]["ModelId"]));
        document.SetTestFeaturesTestPointId(QString::fromStdString(j["TestFeatures"]["TestPointId"]));
        document.SetTestFeaturesAlpha(j["TestFeatures"]["Alpha"]);
        document.SetTestFeaturesBeta(j["TestFeatures"]["Beta"]);

        PspDocumentProcessConfig config;
        from_json(j["ProcessConfig"], config);
        document.SetProcessConfig(config);

        std::vector<MarkerPoint> markers;
        if (j.contains("markersWindOn")) {
            for (auto iter = j["markersWindOn"].begin(); iter != j["markersWindOn"].end(); ++iter) {
                MarkerPoint marker;
                from_json(*iter, marker);
                markers.push_back(marker);
            }
            document.SetWindOnMarkers(markers);
        }

        markers.clear();
        if (j.contains("markersWindOff")) {
            for (auto iter = j["markersWindOff"].begin(); iter != j["markersWindOff"].end(); ++iter) {
                MarkerPoint marker;
                from_json(*iter, marker);
                markers.push_back(marker);
            }
        }
        document.SetWindOffMarkers(markers);

        if (j.contains("DeviationMaxX")) {
            document.SetDeviationMaxX(j["DeviationMaxX"]);
        }
        if (j.contains("DeviationMaxY")) {
            document.SetDeviationMaxY(j["DeviationMaxY"]);
        }
        if (j.contains("DeviationMaxDistance")) {
            document.SetDeviationMaxDistance(j["DeviationMaxDistance"]);
        }
        if (j.contains("DeviationMeanX")) {
            document.SetDeviationMeanX(j["DeviationMeanX"]);
        }
        if (j.contains("DeviationMeanY")) {
            document.SetDeviationMeanY(j["DeviationMeanY"]);
        }
        if (j.contains("DeviationMeanDistance")) {
            document.SetDeviationMeanDistance(j["DeviationMeanDistance"]);
        }
    }

}
