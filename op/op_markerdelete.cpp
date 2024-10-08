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
#include "op/op_markerdelete.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_rect.h"
#include "base/base_markerpoint_sort.h"
#include "manager/work_manager.h"
#include <QDebug>
#include <vector>

using namespace PspNsBase;
using namespace PspNsManager;
using namespace std;

namespace PspNsOp {
    OperatorMarkerDelete::OperatorMarkerDelete()
            : Operator(), mStartPoint(0.0, 0.0), mEndPoint(0.0, 0.0), mStep(0) {

    }

    OperatorMarkerDelete::~OperatorMarkerDelete() {

    }

    void OperatorMarkerDelete::LButtonDown(const QPointF &pt) {
        mStep = 1;

        mStartPoint = pt;
    }

    void OperatorMarkerDelete::LButtonUp(const QPointF &pt) {
        mEndPoint = pt;

        clearMarkerSelected();

        mStep = 0;
        GraphicManager::GetInstance()->DestroyAllGraphics();
    }

    void OperatorMarkerDelete::LButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerDelete::RButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerDelete::RButtonUp(const QPointF &pt) {
        emit PopMenu();
    }

    void OperatorMarkerDelete::RButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerDelete::MButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerDelete::MButtonUp(const QPointF &pt) {

    }

    void OperatorMarkerDelete::MouseMove(const QPointF &pt) {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        if (mStep == 1) {
            mEndPoint = pt;

            GraphicRect *rect = new GraphicRect(mStartPoint, mEndPoint);
            GraphicManager::GetInstance()->AddGraphic(rect);
        }
    }

    void OperatorMarkerDelete::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }


    void OperatorMarkerDelete::SetParam(OperatorParam *param) {

    }

    void OperatorMarkerDelete::EndOperator() {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        SetCtrlPressed(false);
    }

    void OperatorMarkerDelete::clearMarkerSelected() {
        if (mStartPoint.isNull() || mEndPoint.isNull()) {
            return;
        }

        GraphicRect rect(min(mStartPoint.x(), mEndPoint.x()),
                         min(mStartPoint.y(), mEndPoint.y()),
                         fabs(mStartPoint.x() - mEndPoint.x()),
                         fabs(mStartPoint.y() - mEndPoint.y()));
        const vector<MarkerPoint> &markerPoints = WorkImgManager::GetInstance()->getMarkerPoints();
        for (size_t i = 0; i < markerPoints.size(); i++) {
            if (rect.ContainPoint(markerPoints[i].getPoint())) {
                WorkImgManager::GetInstance()->setMarkerPointSelectd(i, true);
            }
        }
        WorkImgManager::GetInstance()->clearMarkerPointSelected();
    }

    void OperatorMarkerDelete::Finish() {
        EndOperator();

        clearMarkerSelected();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);

        mStep = 0;
    }

}
