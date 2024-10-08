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
#include "op/op_factory.h"
#include "op/op_general.h"
#include "op/op_markeradd.h"
#include "op/op_markerdelete.h"
#include "op/op_paint_calibration_range.h"
#include "op/op_sectional_view.h"
#include "op/op_roi_add.h"
#include "op/op_marker_pin.h"
#include "op/op_length_measure.h"
#include "op/op_marker_translate.h"
#include <assert.h>

namespace PspNsOp {

    OperatorFactory::OperatorFactory() {
        if (mOperators.empty()) {
            mOperators.insert(std::make_pair(OperatorType::eOperatorGeneral, new OperatorGeneral()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorMarkerAdd, new OperatorMarkerAdd()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorMarkerDelete, new OperatorMarkerDelete()));
            mOperators.insert(
                    std::make_pair(OperatorType::eOperatorPaintCalibrationRange, new OperatorPaintCalibrationRange()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorSectionalView, new OperatorSectionalView()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorROISet, new OperatorROIAdd()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorMarkerPin, new OperatorMarkerPin()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorLengthMeasure, new OperatorLengthMeasure()));
            mOperators.insert(std::make_pair(OperatorType::eOperatorMarkerTranslate, new OperatorMarkerTranslate()));
        }
    }

    OperatorFactory::~OperatorFactory() {
        for (auto iter: mOperators) {
            if (iter.second) {
                delete iter.second;
                iter.second = nullptr;
            }
        }
    }

    Operator *OperatorFactory::CreateOperator(OperatorType type, OperatorParam *param) {
        auto iter = mOperators.find(type);

        if (mOperators.end() == iter) {
            assert(false);
            return nullptr;
        }

        iter->second->SetParam(param);

        return iter->second;
    }

}
