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
#include "base/base_arithmatic.h"
#include <cmath>
#include <float.h>

namespace PspNsBase {

    bool SolveQuadraticEquation(double a, double b, double c, double &x1, double &x2) {
        if (fabs(a) < DBL_EPSILON || b * b - 4 * a * c < 0.0) {
            assert(false);
            return false;
        }

        double delta = sqrt(b * b - 4 * a * c);

        x1 = (-b + delta) / (2 * a);
        x2 = (-b - delta) / (2 * a);

        return true;
    }

}
