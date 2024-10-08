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
#pragma once

#include "op_global.h"
#include "op_type.h"
#include "op_parameter.h"
#include <QPointF>
#include <QObject>

namespace PspNsOp {
    class OP_SHARED Operator : public QObject {
    Q_OBJECT
    public:
        Operator() : mCtrlPressed(false) {}
        virtual ~Operator() {

        }

        virtual void LButtonDown(const QPointF &pt) {}
        virtual void LButtonUp(const QPointF &pt) {}
        virtual void LButtonDblClk(const QPointF &pt) {}
        virtual void RButtonDown(const QPointF &pt) {}
        virtual void RButtonUp(const QPointF &pt) {}
        virtual void RButtonDblClk(const QPointF &pt) {}
        virtual void MButtonDown(const QPointF &pt) {}
        virtual void MButtonUp(const QPointF &pt) {}
        virtual void MouseMove(const QPointF &pt) {}
        virtual void WheelEvent(const QPointF &pt) {}
        virtual void SetCtrlPressed(bool bCtrlPressed) { mCtrlPressed = bCtrlPressed; }
        virtual bool IsCtrlPressed() { return mCtrlPressed; }

        virtual void Reset() {}

        virtual void SetParam(OperatorParam *param = nullptr) {}
        virtual void EndOperator() {}

        virtual void Finish() {}

    private:
        bool mCtrlPressed;
    };
}
