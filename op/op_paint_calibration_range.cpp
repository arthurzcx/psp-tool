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
#include "op/op_paint_calibration_range.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_rect.h"

using namespace PspNsBase;

namespace PspNsOp {
    OperatorPaintCalibrationRange::OperatorPaintCalibrationRange()
            : Operator(), mStep(0) {

    }

    OperatorPaintCalibrationRange::~OperatorPaintCalibrationRange() {

    }

    void OperatorPaintCalibrationRange::LButtonDown(const QPointF &pt) {
        mStep = 1;

        mStartPoint = pt;
        mEndPoint = pt;
    }

    void OperatorPaintCalibrationRange::LButtonUp(const QPointF &pt) {
        mStep = 0;

        mEndPoint = pt;
        GraphicManager::GetInstance()->DestroyAllGraphics();

        GraphicRect *rect = new GraphicRect(mStartPoint, mEndPoint);
        GraphicManager::GetInstance()->AddGraphic(rect);

        emit signalPaintCalibrationRangeSaveFromGraphicManager();
    }

    void OperatorPaintCalibrationRange::LButtonDblClk(const QPointF &pt) {
    }

    void OperatorPaintCalibrationRange::RButtonDown(const QPointF &pt) {
    }

    void OperatorPaintCalibrationRange::RButtonUp(const QPointF &pt) {
        if (!QRectF(mStartPoint, mEndPoint).isValid())return;
        emit PopMenu();
    }

    void OperatorPaintCalibrationRange::RButtonDblClk(const QPointF &pt) {
    }

    void OperatorPaintCalibrationRange::MButtonDown(const QPointF &pt) {
    }

    void OperatorPaintCalibrationRange::MButtonUp(const QPointF &pt) {
    }

    void OperatorPaintCalibrationRange::MouseMove(const QPointF &pt) {
        if (mStep == 0) return;

        GraphicManager::GetInstance()->DestroyAllGraphics();
        mEndPoint = pt;

        GraphicRect *rect = new GraphicRect(mStartPoint, mEndPoint);
        GraphicManager::GetInstance()->AddGraphic(rect);
    }

    void OperatorPaintCalibrationRange::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }


    void OperatorPaintCalibrationRange::SetParam(OperatorParam *param) {
    }

    void OperatorPaintCalibrationRange::EndOperator() {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        mStep = 0;

        SetCtrlPressed(false);
    }

    void OperatorPaintCalibrationRange::Finish() {
        EndOperator();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }

    const QPointF &OperatorPaintCalibrationRange::GetStartPoint() {
        return mStartPoint;
    }

    const QPointF &OperatorPaintCalibrationRange::GetEndPoint() {
        return mEndPoint;
    }

    void OperatorPaintCalibrationRange::ExitOperator() {
        Finish();
    }
}
