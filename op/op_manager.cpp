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
#include "op/op_manager.h"
#include "op/op_factory.h"
#include <assert.h>

namespace PspNsOp {

    std::map<OperatorType, Operator *> OperatorFactory::mOperators;
    static OperatorFactory _stOperatorFactory_;

    OperatorManager::OperatorManager() : Operator() {
        SetOperator(OperatorType::eOperatorGeneral);
        mIsDraging = false;
    }

    OperatorManager::~OperatorManager() {
    }

    OperatorManager *OperatorManager::GetInstance() {
        static OperatorManager stInstance;
        return &stInstance;
    }

    void OperatorManager::SetOperator(OperatorType type, OperatorParam *param) {
        mOperator = OperatorFactory::CreateOperator(type, param);
        assert(nullptr != mOperator);
        mOperator->Reset();

        mOperatorTypeCurrent = type;
    }

    void OperatorManager::SetCtrlPressed(bool bCtrlPressed) {
        if (mOperator) {
            mOperator->SetCtrlPressed(bCtrlPressed);
        }
    }

    QString OperatorManager::GetOpType() const {
        switch ((int) mOperatorTypeCurrent) {
            case (int) OperatorType::eOperatorGeneral: {
                return tr("");
            }
            case (int) OperatorType::eOperatorMarkerAdd: {
                return tr("Mode: Marker detection");
            }
            case (int) OperatorType::eOperatorMarkerDelete: {
                return tr("Mode: Marker delete");
            }
            case (int) OperatorType::eOperatorMarkerSort: {
                return tr("Mode: Marker sort");
            }
            case (int) OperatorType::eOperatorROISet: {
                return tr("Mode: ROI set");
            }
            case (int) OperatorType::eOperatorROIDelete: {
                return tr("Mode: ROT delete");
            }
            case (int) OperatorType::eOperatorPaintCalibrationRange: {
                return tr("Mode: paint calibration range set");
            }
        }

        return "";
    }


    void OperatorManager::LButtonDown(const QPointF &pt) {
        mOperator->LButtonDown(pt);
    }

    void OperatorManager::LButtonUp(const QPointF &pt) {
        mOperator->LButtonUp(pt);
    }

    void OperatorManager::LButtonDblClk(const QPointF &pt) {
        mOperator->LButtonDblClk(pt);
    }

    void OperatorManager::RButtonDown(const QPointF &pt) {
        mOperator->RButtonDown(pt);
    }

    void OperatorManager::RButtonUp(const QPointF &pt) {
        mOperator->RButtonUp(pt);
    }

    void OperatorManager::RButtonDblClk(const QPointF &pt) {
        mOperator->RButtonDblClk(pt);
    }

    void OperatorManager::MButtonDown(const QPointF &pt) {
        mIsDraging = true;
        mOperator->MButtonDown(pt);
    }

    void OperatorManager::MButtonUp(const QPointF &pt) {
        mOperator->MButtonUp(pt);
        mIsDraging = false;
    }

    void OperatorManager::MouseMove(const QPointF &pt) {
        if (mIsDraging) {
        }
        mOperator->MouseMove(pt);
    }

    void OperatorManager::WheelEvent(const QPointF &pt) {
        mOperator->WheelEvent(pt);
    }

    void OperatorManager::Finish() {
        mOperator->Finish();
    }
}
