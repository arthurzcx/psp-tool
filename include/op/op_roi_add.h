﻿/*
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
#pragma once

#include "op_global.h"
#include "op_type.h"
#include "op_parameter.h"
#include "op_base.h"
#include "base/base_graphic.h"

using namespace PspNsBase;

namespace PspNsOp {
    enum eROIType {
        eBox = 0,
        ePolygon
    };

    class OP_SHARED OperatorROIAdd : public Operator {
    Q_OBJECT
    public:
        OperatorROIAdd();
        ~OperatorROIAdd();

        void LButtonDown(const QPointF &pt) override;
        void LButtonUp(const QPointF &pt) override;
        void LButtonDblClk(const QPointF &pt) override;
        void RButtonDown(const QPointF &pt) override;
        void RButtonUp(const QPointF &pt) override;
        void RButtonDblClk(const QPointF &pt) override;
        void MButtonDown(const QPointF &pt) override;
        void MButtonUp(const QPointF &pt) override;
        void MouseMove(const QPointF &pt) override;
        void WheelEvent(const QPointF &pt) override;

        void SetParam(OperatorParam *param = nullptr) override;
        void EndOperator() override;
        void Finish() override;

        void SetROIType(eROIType type);
        eROIType GetROIType();

    public slots:
        void ExitOperator();

    signals:
        void PopMenu();

    private:
        Graphic *mROI;
        eROIType mROIType;

        QPointF mStartPoint;
        std::vector<QPointF> mPolygonPoints;
        int mStep;
    };
}
