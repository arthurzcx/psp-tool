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
#include <map>

namespace PspNsOp {
    class OP_SHARED OperatorFactory {
    public:
        OperatorFactory();
        ~OperatorFactory();

        static Operator *CreateOperator(OperatorType type, OperatorParam *param = nullptr);

    private:
        static std::map<OperatorType, Operator *> mOperators;
    };
}