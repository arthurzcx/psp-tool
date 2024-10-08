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
#include "op/op_markeradd.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_rect.h"
#include "manager/work_manager.h"

using namespace PspNsBase;
using namespace PspNsManager;

namespace PspNsOp {
    OperatorMarkerAdd::OperatorMarkerAdd()
            : Operator(), mBoxHalfSize(7) {

    }

    OperatorMarkerAdd::~OperatorMarkerAdd() {

    }

    void OperatorMarkerAdd::LButtonDown(const QPointF &pt) {
        cv::Rect roiRect(pt.x() - mBoxHalfSize, pt.y() - mBoxHalfSize, mBoxHalfSize * 2, mBoxHalfSize * 2);
        WorkImgManager::GetInstance()->detectMarkerPtByROI(roiRect);
    }

    void OperatorMarkerAdd::LButtonUp(const QPointF &pt) {

    }

    void OperatorMarkerAdd::LButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerAdd::RButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerAdd::RButtonUp(const QPointF &pt) {
        emit PopMenu();
    }

    void OperatorMarkerAdd::RButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerAdd::MButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerAdd::MButtonUp(const QPointF &pt) {

    }

    void OperatorMarkerAdd::MouseMove(const QPointF &pt) {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        GraphicRect *rect = new GraphicRect(pt.x() - mBoxHalfSize, pt.y() - mBoxHalfSize,
                                            mBoxHalfSize * 2, mBoxHalfSize * 2);
        GraphicManager::GetInstance()->AddGraphic(rect);
    }

    void OperatorMarkerAdd::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }


    void OperatorMarkerAdd::SetParam(OperatorParam *param) {

    }

    void OperatorMarkerAdd::EndOperator() {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        SetCtrlPressed(false);
    }

    void OperatorMarkerAdd::Finish() {
        EndOperator();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }

    void OperatorMarkerAdd::SetBoxSize(int size) {
        if (size < 2) {
            assert(false);
        }

        mBoxHalfSize = size / 2;
    }

    int OperatorMarkerAdd::GetBoxSize() {
        return 2 * mBoxHalfSize;
    }

    void OperatorMarkerAdd::ExitOperator() {
        Finish();
    }
}
