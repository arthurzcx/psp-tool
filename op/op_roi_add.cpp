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
#include "op/op_roi_add.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_rect.h"
#include "base/base_graphic_polygon.h"
#include "manager/work_manager.h"

using namespace PspNsBase;
using namespace PspNsManager;

namespace PspNsOp {
    OperatorROIAdd::OperatorROIAdd()
            : Operator(), mROI(nullptr), mROIType(eROIType::eBox), mStep(0) {

    }

    OperatorROIAdd::~OperatorROIAdd() {

    }

    void OperatorROIAdd::LButtonDown(const QPointF &pt) {
        if (mStep == 0) {
            mStartPoint = pt;

            if (eROIType::eBox == mROIType) {
                mROI = new GraphicRect(mStartPoint, mStartPoint);
            } else if (eROIType::ePolygon == mROIType) {
                mPolygonPoints.push_back(mStartPoint);
                mROI = new GraphicPolygon(mPolygonPoints);
            }

            GraphicManager::GetInstance()->AddGraphic(mROI);

            mStep = 1;
        } else if (eROIType::ePolygon == mROIType) {
            if (mStep == 2)
                mPolygonPoints.pop_back();

            mPolygonPoints.push_back(pt);
            GraphicManager::GetInstance()->DestroyAllGraphics();
            mROI = new GraphicPolygon(mPolygonPoints);

            GraphicManager::GetInstance()->AddGraphic(mROI);

            mStep = 1;
        }
    }

    void OperatorROIAdd::LButtonUp(const QPointF &pt) {
        if (mStep == 1) {
            if (eROIType::eBox == mROIType) {
                GraphicManager::GetInstance()->DestroyAllGraphics();
                mROI = new GraphicRect(mStartPoint, pt);

                Finish();
            }
        }
    }

    void OperatorROIAdd::LButtonDblClk(const QPointF &pt) {
    }

    void OperatorROIAdd::RButtonDown(const QPointF &pt) {
    }

    void OperatorROIAdd::RButtonUp(const QPointF &pt) {
        Finish();
    }

    void OperatorROIAdd::RButtonDblClk(const QPointF &pt) {
    }

    void OperatorROIAdd::MButtonDown(const QPointF &pt) {
    }

    void OperatorROIAdd::MButtonUp(const QPointF &pt) {
    }

    void OperatorROIAdd::MouseMove(const QPointF &pt) {
        if (mStep == 1) {
            GraphicManager::GetInstance()->DestroyAllGraphics();
            if (eROIType::eBox == mROIType) {
                mROI = new GraphicRect(mStartPoint, pt);

            } else if (eROIType::ePolygon == mROIType) {
                mPolygonPoints.push_back(pt);
                mROI = new GraphicPolygon(mPolygonPoints);
                mStep = 2;
            } else {
                assert(false);
            }
            GraphicManager::GetInstance()->AddGraphic(mROI);
        } else if (mStep == 2) {
            mPolygonPoints.pop_back();
            GraphicManager::GetInstance()->DestroyAllGraphics();
            mPolygonPoints.push_back(pt);
            mROI = new GraphicPolygon(mPolygonPoints);
            GraphicManager::GetInstance()->AddGraphic(mROI);
        }
    }

    void OperatorROIAdd::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }

    void OperatorROIAdd::SetParam(OperatorParam *param) {

    }

    void OperatorROIAdd::EndOperator() {
        mStep = 0;

        if (mROI) {
            if (GraphicPolygon *pPolygon = dynamic_cast<GraphicPolygon *>(mROI)) {
                pPolygon->MakeConvex();
            }
            WorkImgManager::GetInstance()->addROI(mROI);

            mROI = nullptr;

            GraphicManager::GetInstance()->Clear();
        }

        mPolygonPoints.clear();
    }

    void OperatorROIAdd::Finish() {
        EndOperator();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }

    void OperatorROIAdd::SetROIType(eROIType type) {
        mROIType = type;

        if (type != eROIType::eBox && type != eROIType::ePolygon) {
            assert(false);
        }
    }

    eROIType OperatorROIAdd::GetROIType() {
        return mROIType;
    }

    void OperatorROIAdd::ExitOperator() {
        Finish();
    }
}
