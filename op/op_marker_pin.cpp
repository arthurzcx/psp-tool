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
#include "op/op_marker_pin.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_rect.h"
#include "base/base_graphic_line.h"
#include "manager/work_manager.h"

using namespace PspNsBase;
using namespace PspNsManager;

namespace PspNsOp {
    OperatorMarkerPin::OperatorMarkerPin()
            : Operator(), mBoxHalfSize(7), mStep(0) {

    }

    OperatorMarkerPin::~OperatorMarkerPin() {

    }

    void OperatorMarkerPin::LButtonDown(const QPointF &pt) {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        GraphicLine *line0 = new GraphicLine(QPointF(pt.x() - mBoxHalfSize, pt.y() - mBoxHalfSize),
                                             QPointF(pt.x() + mBoxHalfSize, pt.y() + mBoxHalfSize));
        GraphicLine *line1 = new GraphicLine(QPointF(pt.x() - mBoxHalfSize, pt.y() + mBoxHalfSize),
                                             QPointF(pt.x() + mBoxHalfSize, pt.y() - mBoxHalfSize));

        GraphicManager::GetInstance()->AddGraphic(line0);
        GraphicManager::GetInstance()->AddGraphic(line1);

        WorkImgManager::GetInstance()->addMarkerPoint(MarkerPoint(pt, mBoxHalfSize));
    }

    void OperatorMarkerPin::LButtonUp(const QPointF &pt) {

    }

    void OperatorMarkerPin::LButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerPin::RButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerPin::RButtonUp(const QPointF &pt) {
        Finish();

        emit PopMenu();
    }

    void OperatorMarkerPin::RButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerPin::MButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerPin::MButtonUp(const QPointF &pt) {

    }

    void OperatorMarkerPin::MouseMove(const QPointF &pt) {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        GraphicLine *line0 = new GraphicLine(QPointF(pt.x() - mBoxHalfSize, pt.y() - mBoxHalfSize),
                                             QPointF(pt.x() + mBoxHalfSize, pt.y() + mBoxHalfSize));
        GraphicLine *line1 = new GraphicLine(QPointF(pt.x() - mBoxHalfSize, pt.y() + mBoxHalfSize),
                                             QPointF(pt.x() + mBoxHalfSize, pt.y() - mBoxHalfSize));

        GraphicManager::GetInstance()->AddGraphic(line0);
        GraphicManager::GetInstance()->AddGraphic(line1);
    }

    void OperatorMarkerPin::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }

    void OperatorMarkerPin::SetParam(OperatorParam *param) {

    }

    void OperatorMarkerPin::EndOperator() {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        SetCtrlPressed(false);

        mStep = 0;
    }

    void OperatorMarkerPin::Finish() {
        EndOperator();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }

    void OperatorMarkerPin::SetBoxSize(int size) {
        if (size < 2) {
            assert(false);
        }

        mBoxHalfSize = size / 2;
    }

    int OperatorMarkerPin::GetBoxSize() {
        return 2 * mBoxHalfSize;
    }

    void OperatorMarkerPin::ExitOperator() {
        Finish();
    }
}
