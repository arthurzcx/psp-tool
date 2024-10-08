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
#include "op/op_sectional_view.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_line.h"

using namespace PspNsBase;

namespace PspNsOp {
    OperatorSectionalView::OperatorSectionalView()
            : Operator(), mStartPoint(QPointF(0.0, 0.0)), mEndPoint(QPointF(0.0, 0.0)), mStep(0) {

    }

    OperatorSectionalView::~OperatorSectionalView() {

    }

    void OperatorSectionalView::LButtonDown(const QPointF &pt) {
        mStep = 1;
        mStartPoint = pt;
    }

    void OperatorSectionalView::LButtonUp(const QPointF &pt) {
        mEndPoint = pt;

        if (IsCtrlPressed()) {
            if (abs(mEndPoint.y() - mStartPoint.y()) >= abs(mEndPoint.x() - mStartPoint.x())) {
                mEndPoint.setX(mStartPoint.rx());
            } else {
                mEndPoint.setY(mStartPoint.ry());
            }
        }

        emit drawSectionalView(mStartPoint, mEndPoint);

        Finish();
    }

    void OperatorSectionalView::LButtonDblClk(const QPointF &pt) {

    }

    void OperatorSectionalView::RButtonDown(const QPointF &pt) {
        Finish();
    }

    void OperatorSectionalView::RButtonUp(const QPointF &pt) {

    }

    void OperatorSectionalView::RButtonDblClk(const QPointF &pt) {

    }

    void OperatorSectionalView::MButtonDown(const QPointF &pt) {

    }

    void OperatorSectionalView::MButtonUp(const QPointF &pt) {

    }

    void OperatorSectionalView::MouseMove(const QPointF &pt) {
        if (mStep == 1) {
            mEndPoint = pt;

            if (IsCtrlPressed()) {
                if (abs(mEndPoint.y() - mStartPoint.y()) >= abs(mEndPoint.x() - mStartPoint.x())) {
                    mEndPoint.setX(mStartPoint.rx());
                } else {
                    mEndPoint.setY(mStartPoint.ry());
                }
            }

            GraphicManager::GetInstance()->DestroyAllGraphics();
            GraphicManager::GetInstance()->AddGraphic(new GraphicLine(mStartPoint, mEndPoint));
        }
    }

    void OperatorSectionalView::WheelEvent(const QPointF &pt) {

    }

    void OperatorSectionalView::SetParam(OperatorParam *param) {

    }

    void OperatorSectionalView::EndOperator() {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        mStep = 0;

        SetCtrlPressed(false);
    }

    void OperatorSectionalView::Finish() {
        EndOperator();
        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }
}
