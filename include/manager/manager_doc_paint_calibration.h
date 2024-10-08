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
#include "base/base_graphic_rect.h"
#include "base/base_def.h"

namespace PspNsManager {
    // Describe the information of experiment.
    // The variable should be added when needed.
    struct MANAGER_SHARED PspTestFeaturesPaintCalibration {
        QString mTestName;
        QString mTestModelName;
        QString mTestPointName;
        QString mTestPaintName;
        QString mTestDateTime;
        PspNsBase::eTypeFormula mTestTypeFormula;
        QString mTestDesc;

        PspNsBase::eUnitPressure mUnitPressure;
        double mPressureNormalizing;
        double mPressureMinimal;
        double mPressureMaximal;
        double mPressureTolerance;

        PspNsBase::eUnitTemperature mUnitTemperature;
        double mTemperatureNormalizing;
        double mTemperatureMinimal;
        double mTemperatureMaximal;
        double mTemperatureTolerance;

        std::vector<double> mCalibrationCoefficient;

        PspTestFeaturesPaintCalibration();
        ~PspTestFeaturesPaintCalibration();

        PspTestFeaturesPaintCalibration(const PspTestFeaturesPaintCalibration &features);
        PspTestFeaturesPaintCalibration &operator=(const PspTestFeaturesPaintCalibration &features);
    };

    MANAGER_SHARED void to_json(Json &j, const PspTestFeaturesPaintCalibration &features);
    MANAGER_SHARED void from_json(const Json &j, PspTestFeaturesPaintCalibration &features);

    struct MANAGER_SHARED PspDataPaintCalibration {
        QString mImagePath;
        double mPressure;
        double mTemperature;
        bool mIsBackground;
        bool mIsReference;
        cv::Mat mImage;
    };

    MANAGER_SHARED void to_json(Json &j, const PspDataPaintCalibration &data);
    MANAGER_SHARED void from_json(const Json &j, PspDataPaintCalibration &data);

    class MANAGER_SHARED PspDocPaintCalibration : public PspDocument {
    public:
        PspDocPaintCalibration();
        virtual ~PspDocPaintCalibration();

        PspDocPaintCalibration(const PspDocPaintCalibration &document);
        PspDocPaintCalibration &operator=(const PspDocPaintCalibration &document);

        const PspTestFeaturesPaintCalibration &GetTestFeatures() const;
        void SetTestFeatures(const PspTestFeaturesPaintCalibration &features);

        const std::vector<PspDataPaintCalibration> &GetData() const;
        void SetData(const std::vector<PspDataPaintCalibration> &data);
        void AddData(const PspDataPaintCalibration &data);
        void RemoveData(int index);

        const PspNsBase::GraphicRect &GetRoi() const;
        void SetRoi(const PspNsBase::GraphicRect &rect);
    private:
        PspTestFeaturesPaintCalibration mTestFeatures;
        std::vector<PspDataPaintCalibration> mData;
        PspNsBase::GraphicRect mRoiRect;
    };

    MANAGER_SHARED void to_json(Json &j, const PspDocPaintCalibration &document);
    MANAGER_SHARED void from_json(const Json &j, PspDocPaintCalibration &document);
}
