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
#include "op_base.h"

namespace PspNsOp {

    class OP_SHARED OperatorManager : public Operator {
        OperatorManager();
    public:
        ~OperatorManager();
        OperatorManager(const OperatorManager &) = delete;
        OperatorManager &operator=(const OperatorManager &) = delete;

        static OperatorManager *GetInstance();

        void SetOperator(OperatorType type, OperatorParam *param = nullptr);
        QString GetOpType() const;

        inline Operator *GetOperator() {
            return mOperator;
        }

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

        void SetCtrlPressed(bool bCtrlPressed) override;
        void Finish() override;

        inline bool IsDraging() {
            return mIsDraging;
        }

    private:
        Operator *mOperator;
        bool mIsDraging;
        OperatorType mOperatorTypeCurrent;
    };
}
