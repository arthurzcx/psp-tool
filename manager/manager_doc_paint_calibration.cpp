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
#include "manager/manager_doc_paint_calibration.h"

namespace PspNsManager {
    PspTestFeaturesPaintCalibration::PspTestFeaturesPaintCalibration()
            : mTestName(""), mTestModelName(""), mTestPointName(""), mTestPaintName(""), mTestDateTime(""),
              mTestTypeFormula(PspNsBase::eTypeFormula::eQuadratic), mTestDesc(""),
              mUnitPressure(PspNsBase::eUnitPressure::ePa), mPressureNormalizing(0.0), mPressureMinimal(0.0),
              mPressureMaximal(0.0), mPressureTolerance(0.0), mUnitTemperature(PspNsBase::eUnitTemperature::eK),
              mTemperatureNormalizing(0.0), mTemperatureMinimal(0.0), mTemperatureMaximal(0.0),
              mTemperatureTolerance(0.0) {

    }
    PspTestFeaturesPaintCalibration::~PspTestFeaturesPaintCalibration() {

    }

    PspTestFeaturesPaintCalibration::PspTestFeaturesPaintCalibration(const PspTestFeaturesPaintCalibration &features)
            : mTestName(features.mTestName), mTestModelName(features.mTestModelName),
              mTestPointName(features.mTestPointName), mTestPaintName(features.mTestPaintName),
              mTestDateTime(features.mTestDateTime), mTestTypeFormula(features.mTestTypeFormula),
              mTestDesc(features.mTestDesc), mUnitPressure(features.mUnitPressure),
              mPressureNormalizing(features.mPressureNormalizing), mPressureMinimal(features.mPressureMinimal),
              mPressureMaximal(features.mPressureMaximal), mPressureTolerance(features.mPressureTolerance),
              mUnitTemperature(features.mUnitTemperature), mTemperatureNormalizing(features.mTemperatureNormalizing),
              mTemperatureMinimal(features.mTemperatureMinimal), mTemperatureMaximal(features.mTemperatureMaximal),
              mTemperatureTolerance(features.mTemperatureTolerance),
              mCalibrationCoefficient(features.mCalibrationCoefficient) {

    }
    PspTestFeaturesPaintCalibration &
    PspTestFeaturesPaintCalibration::operator=(const PspTestFeaturesPaintCalibration &features) {
        mTestName = features.mTestName;
        mTestModelName = features.mTestModelName;
        mTestPointName = features.mTestPointName;
        mTestPaintName = features.mTestPaintName;
        mTestDateTime = features.mTestDateTime;
        mTestTypeFormula = features.mTestTypeFormula;
        mTestDesc = features.mTestDesc;

        mUnitPressure = features.mUnitPressure;
        mPressureMinimal = features.mPressureMinimal;
        mPressureMaximal = features.mPressureMaximal;
        mPressureTolerance = features.mPressureTolerance;
        mPressureNormalizing = features.mPressureNormalizing;

        mUnitTemperature = features.mUnitTemperature;
        mTemperatureNormalizing = features.mTemperatureNormalizing;
        mTemperatureMinimal = features.mTemperatureMinimal;
        mTemperatureMaximal = features.mTemperatureMaximal;
        mTemperatureTolerance = features.mTemperatureTolerance;

        mCalibrationCoefficient = features.mCalibrationCoefficient;

        return *this;
    }

    MANAGER_SHARED void to_json(Json &j, const PspTestFeaturesPaintCalibration &features) {
        j["TestName"] = features.mTestName.toStdString();
        j["TestModelName"] = features.mTestModelName.toStdString();
        j["TestPointName"] = features.mTestPointName.toStdString();
        j["TestPaintName"] = features.mTestPaintName.toStdString();
        j["TestDateTime"] = features.mTestDateTime.toStdString();
        j["TestTypeFormula"] = PspNsBase::TypeFormulaToString(features.mTestTypeFormula).toStdString();
        j["TestDesc"] = features.mTestDesc.toStdString();

        j["TestUnitPressure"] = PspNsBase::PressureToString(features.mUnitPressure).toStdString();
        j["PressureNormalizing"] = features.mPressureNormalizing;
        j["PressureMinimal"] = features.mPressureMinimal;
        j["PressureMaximal"] = features.mPressureMaximal;
        j["PressureTolerance"] = features.mPressureTolerance;

        j["TestUnitTemperature"] = PspNsBase::TemperatureToString(features.mUnitTemperature).toStdString();
        j["TemperatureNormalizing"] = features.mTemperatureNormalizing;
        j["TemperatureMinimal"] = features.mTemperatureMinimal;
        j["TemperatureMaximal"] = features.mTemperatureMaximal;
        j["TemperatureTolerance"] = features.mTemperatureTolerance;

        for (auto coeffieicent: features.mCalibrationCoefficient) {
            j["CalibrationCoefficient"].push_back(coeffieicent);
        }
    }

    MANAGER_SHARED void from_json(const Json &j, PspTestFeaturesPaintCalibration &features) {
        if (j.contains("TestName")) {
            features.mTestName = QString::fromStdString(j["TestName"]);
        }

        if (j.contains("TestModelName")) {
            features.mTestModelName = QString::fromStdString(j["TestModelName"]);
        }

        if (j.contains("TestPointName")) {
            features.mTestPointName = QString::fromStdString(j["TestPointName"]);
        }

        if (j.contains("TestPaintName")) {
            features.mTestPaintName = QString::fromStdString(j["TestPaintName"]);
        }

        if (j.contains("TestDateTime")) {
            features.mTestDateTime = QString::fromStdString(j["TestDateTime"]);
        }

        if (j.contains("TestTypeFormula")) {
            features.mTestTypeFormula = PspNsBase::TypeFormulaFromString(QString::fromStdString(j["TestTypeFormula"]));
        }

        if (j.contains("TestDesc")) {
            features.mTestDesc = QString::fromStdString(j["TestDesc"]);
        }

        if (j.contains("TestUnitPressure")) {
            features.mUnitPressure = PspNsBase::PressureFromString(QString::fromStdString(j["TestUnitPressure"]));
        }

        if (j.contains("TestUnitTemperature")) {
            features.mUnitTemperature = PspNsBase::TemperatureFromString(
                    QString::fromStdString(j["TestUnitTemperature"]));
        }

        if (j.contains("PressureNormalizing")) {
            features.mPressureNormalizing = j["PressureNormalizing"];
        }
        if (j.contains("PressureMinimal")) {
            features.mPressureMinimal = j["PressureMinimal"];
        }
        if (j.contains("PressureMaximal")) {
            features.mPressureMaximal = j["PressureMaximal"];
        }
        if (j.contains("PressureTolerance")) {
            features.mPressureTolerance = j["PressureTolerance"];
        }
        if (j.contains("TemperatureNormalizing")) {
            features.mTemperatureNormalizing = j["TemperatureNormalizing"];
        }
        if (j.contains("TemperatureMinimal")) {
            features.mTemperatureMinimal = j["TemperatureMinimal"];
        }
        if (j.contains("TemperatureMaximal")) {
            features.mTemperatureMaximal = j["TemperatureMaximal"];
        }
        if (j.contains("TemperatureTolerance")) {
            features.mTemperatureTolerance = j["TemperatureTolerance"];
        }

        features.mCalibrationCoefficient.clear();
        if (j.contains("CalibrationCoefficient")) {
            for (auto iter = j["CalibrationCoefficient"].begin(); iter != j["CalibrationCoefficient"].end(); ++iter) {
                features.mCalibrationCoefficient.push_back(*iter);
            }
        }
    }

    MANAGER_SHARED void to_json(Json &j, const PspDataPaintCalibration &data) {
        j["ImagePath"] = data.mImagePath.toStdString();
        j["Pressure"] = data.mPressure;
        j["Temperature"] = data.mTemperature;

        j["IsBackground"] = data.mIsBackground;
        j["IsReference"] = data.mIsReference;
    }

    MANAGER_SHARED void from_json(const Json &j, PspDataPaintCalibration &data) {
        if (j.contains("ImagePath")) {
            data.mImagePath = QString::fromStdString(j["ImagePath"]);
        }

        if (j.contains("Pressure")) {
            data.mPressure = j["Pressure"];
        }

        if (j.contains("Temperature")) {
            data.mTemperature = j["Temperature"];
        }

        if (j.contains("IsBackground")) {
            data.mIsBackground = j["IsBackground"];
        }

        if (j.contains("IsReference")) {
            data.mIsReference = j["IsReference"];
        }
    }

    PspDocPaintCalibration::PspDocPaintCalibration() {
    }

    PspDocPaintCalibration::~PspDocPaintCalibration() {
    }

    PspDocPaintCalibration::PspDocPaintCalibration(const PspDocPaintCalibration &document)
            : mTestFeatures(document.mTestFeatures), mData(document.mData), mRoiRect(document.mRoiRect) {

    }

    PspDocPaintCalibration &PspDocPaintCalibration::operator=(const PspDocPaintCalibration &document) {
        mTestFeatures = document.mTestFeatures;
        mData = document.mData;
        mRoiRect = document.mRoiRect;

        return *this;
    }

    const PspTestFeaturesPaintCalibration &PspDocPaintCalibration::GetTestFeatures() const {
        return mTestFeatures;
    }

    void PspDocPaintCalibration::SetTestFeatures(const PspTestFeaturesPaintCalibration &features) {
        mTestFeatures = features;
    }

    const std::vector<PspDataPaintCalibration> &PspDocPaintCalibration::GetData() const {
        return mData;
    }

    void PspDocPaintCalibration::SetData(const std::vector<PspDataPaintCalibration> &data) {
        mData = data;
    }

    void PspDocPaintCalibration::AddData(const PspDataPaintCalibration &data) {
        mData.push_back(data);
    }

    void PspDocPaintCalibration::RemoveData(int index) {
        if (index < 0 || index >= (int) mData.size()) return;

        mData.erase(mData.begin() + index);
    }

    const PspNsBase::GraphicRect &PspDocPaintCalibration::GetRoi() const {
        return mRoiRect;
    }

    void PspDocPaintCalibration::SetRoi(const PspNsBase::GraphicRect &rect) {
        mRoiRect = rect;
    }

    MANAGER_SHARED void to_json(Json &j, const PspDocPaintCalibration &document) {
        j["Features"] = document.GetTestFeatures();

        for (auto iter = document.GetData().begin(); iter != document.GetData().end(); ++iter) {
            Json jData = *iter;
            j["Data"].push_back(jData);
        }

        PspNsBase::to_json(j["Roi"], document.GetRoi());
    }

    MANAGER_SHARED void from_json(const Json &j, PspDocPaintCalibration &document) {
        if (j.contains("Features")) {
            document.SetTestFeatures(j["Features"]);
        }

        if (j.contains("Data")) {
            std::vector<PspDataPaintCalibration> datas;
            for (auto iter = j["Data"].begin(); iter != j["Data"].end(); ++iter) {
                PspDataPaintCalibration data;
                from_json(*iter, data);
                datas.push_back(data);
            }
            document.SetData(datas);
        }

        if (j.contains("Roi")) {
            PspNsBase::GraphicRect roi;
            PspNsBase::from_json(j["Roi"], roi);
            document.SetRoi(roi);
        }
    }
}
