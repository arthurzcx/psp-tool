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

#include "base_global.h"
#include <QString>

namespace PspNsBase {
#define EPSILON 1.0E-7
#define MAX_VALUE 1.7976931348623158e+308
#define MIN_VALUE -1.7976931348623158e+308

    enum eUnitPressure {
        ePa = 0,
        eKPa,
        eBar,
        eMPa,
        eGPa,
        eAtm
    };

    BASE_SHARED double PressureKPa2Pa(double KPa);
    BASE_SHARED double PressurePa2KPa(double pa);
    BASE_SHARED double PressureConvert(double in, eUnitPressure inType, eUnitPressure outType);
    BASE_SHARED QString PressureToString(eUnitPressure type);
    BASE_SHARED eUnitPressure PressureFromString(QString s);

    enum eUnitTemperature {
        eK = 0,
        eC,
        eF
    };
    BASE_SHARED double TemperatureC2K(double c);
    BASE_SHARED double TemperatureK2C(double k);
    BASE_SHARED double TemperatureConvert(double in, eUnitTemperature inType, eUnitTemperature outType);
    BASE_SHARED QString TemperatureToString(eUnitTemperature type);
    BASE_SHARED eUnitTemperature TemperatureFromString(QString s);

    enum eTypeFormula {
        eLinear = 0,
        eQuadratic,
        eCubic
    };
    BASE_SHARED QString TypeFormulaToString(eTypeFormula type);
    BASE_SHARED eTypeFormula TypeFormulaFromString(QString s);
}
