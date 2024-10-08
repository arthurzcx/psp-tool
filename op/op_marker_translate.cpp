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
#include "op/op_marker_translate.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_rect.h"
#include "manager/work_manager.h"

using namespace PspNsBase;
using namespace PspNsManager;

namespace PspNsOp {
    OperatorMarkerTranslate::OperatorMarkerTranslate()
            : Operator(), mStep(0), mMarkerIndex(-1) {

    }

    OperatorMarkerTranslate::~OperatorMarkerTranslate() {

    }

    void OperatorMarkerTranslate::LButtonDown(const QPointF &pt) {
        mMarkerIndex = -1;

        const std::vector<MarkerPoint> &markers = WorkImgManager::GetInstance()->getMarkerPoints();

        for (size_t i = 0; i < markers.size(); i++) {
            const MarkerPoint &marker = markers[i];
            QPointF offsetPoint = QPointF(marker.getDetectHalfWidth(), marker.getDetectHalfWidth());
            GraphicRect rt(marker.getPoint() - offsetPoint, marker.getPoint() + offsetPoint);

            if (rt.ContainPoint(pt)) {
                mMarkerIndex = (int) i;
                WorkImgManager::GetInstance()->setMarkerPointSelectd(i, true);
                break;
            }
        }
    }

    void OperatorMarkerTranslate::LButtonUp(const QPointF &pt) {
        if (mMarkerIndex != -1) {
            MarkerPoint marker = WorkImgManager::GetInstance()->getMarkerPoints()[mMarkerIndex];
            marker.setPoint(pt);
            marker.setSelected(false);

            WorkImgManager::GetInstance()->replaceMarkerPoint(mMarkerIndex, marker);
        }

        mMarkerIndex = -1;
    }

    void OperatorMarkerTranslate::LButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerTranslate::RButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerTranslate::RButtonUp(const QPointF &pt) {
        Finish();

        emit PopMenu();
    }

    void OperatorMarkerTranslate::RButtonDblClk(const QPointF &pt) {

    }

    void OperatorMarkerTranslate::MButtonDown(const QPointF &pt) {

    }

    void OperatorMarkerTranslate::MButtonUp(const QPointF &pt) {

    }

    void OperatorMarkerTranslate::MouseMove(const QPointF &pt) {
        if (mMarkerIndex != -1) {
            MarkerPoint marker = WorkImgManager::GetInstance()->getMarkerPoints()[mMarkerIndex];

            marker.setPoint(pt);
            WorkImgManager::GetInstance()->replaceMarkerPoint(mMarkerIndex, marker);
        }
    }

    void OperatorMarkerTranslate::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }

    void OperatorMarkerTranslate::SetParam(OperatorParam *param) {

    }

    void OperatorMarkerTranslate::EndOperator() {
        mMarkerIndex = -1;
        SetCtrlPressed(false);
    }

    void OperatorMarkerTranslate::Finish() {
        EndOperator();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }

    void OperatorMarkerTranslate::ExitOperator() {
        Finish();
    }
}
