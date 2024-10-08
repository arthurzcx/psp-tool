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
#include "op/op_length_measure.h"
#include "op/op_manager.h"
#include "base/base_graphic_manager.h"
#include "base/base_graphic_text.h"
#include "base/base_graphic_line.h"
#include "manager/work_manager.h"

using namespace PspNsBase;
using namespace PspNsManager;

namespace PspNsOp {
    OperatorLengthMeasure::OperatorLengthMeasure()
            : Operator(), mStep(0) {

    }

    OperatorLengthMeasure::~OperatorLengthMeasure() {

    }

    void OperatorLengthMeasure::drawInfo(const QPointF &pt) {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        if (mStep == 0) {
            QString infoPoint = "(" + QString::number(pt.x()) + ", " + QString::number(pt.y()) + ")";
            GraphicText *pText = new GraphicText(pt, QStringList(infoPoint));
            GraphicManager::GetInstance()->AddGraphic(pText);
        } else if (mStep == 1) {
            QString infoPointStart =
                    "(" + QString::number(mStartPoint.x()) + ", " + QString::number(mStartPoint.y()) + ")";
            GraphicText *pText = new GraphicText(mStartPoint, QStringList(infoPointStart));
            GraphicManager::GetInstance()->AddGraphic(pText);

            QString infpoPointEnd = "(" + QString::number(pt.x()) + ", " + QString::number(pt.y()) + ")";
            pText = new GraphicText(pt, QStringList(infpoPointEnd));
            GraphicManager::GetInstance()->AddGraphic(pText);

            GraphicManager::GetInstance()->AddGraphic(new GraphicLine(mStartPoint, pt));

            QPointF center = (mStartPoint + pt) / 2.0;
            double dis = sqrt(pow(pt.x() - mStartPoint.x(), 2.0) + pow(pt.y() - mStartPoint.y(), 2.0));
            QString infoLength = "Length: " + QString::number(dis);
            GraphicManager::GetInstance()->AddGraphic(new GraphicText(center, QStringList(infoLength)));
        }
    }

    void OperatorLengthMeasure::LButtonDown(const QPointF &pt) {
        drawInfo(pt);

        if (mStep == 0) {
            mStartPoint = pt;

            mStep = 1;
        } else if (mStep == 1) {
            mStep = 2;
        }
    }

    void OperatorLengthMeasure::LButtonUp(const QPointF &pt) {
        if (mStep == 2) {
            Finish();
        }
    }

    void OperatorLengthMeasure::LButtonDblClk(const QPointF &pt) {

    }

    void OperatorLengthMeasure::RButtonDown(const QPointF &pt) {

    }

    void OperatorLengthMeasure::RButtonUp(const QPointF &pt) {
        Finish();
    }

    void OperatorLengthMeasure::RButtonDblClk(const QPointF &pt) {

    }

    void OperatorLengthMeasure::MButtonDown(const QPointF &pt) {

    }

    void OperatorLengthMeasure::MButtonUp(const QPointF &pt) {

    }

    void OperatorLengthMeasure::MouseMove(const QPointF &pt) {
        drawInfo(pt);
    }

    void OperatorLengthMeasure::WheelEvent(const QPointF &pt) {
        MouseMove(pt);
    }


    void OperatorLengthMeasure::SetParam(OperatorParam *param) {

    }

    void OperatorLengthMeasure::EndOperator() {
        GraphicManager::GetInstance()->DestroyAllGraphics();

        SetCtrlPressed(false);

        mStep = 0;
    }

    void OperatorLengthMeasure::Finish() {
        EndOperator();

        OperatorManager::GetInstance()->SetOperator(OperatorType::eOperatorGeneral);
    }

    void OperatorLengthMeasure::ExitOperator() {
        Finish();
    }
}
