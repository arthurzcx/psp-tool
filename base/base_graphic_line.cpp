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
#include "base/base_graphic_line.h"

namespace PspNsBase {
    GraphicLine::GraphicLine() {
        mStart = QPointF(0.0, 0.0);
        mEnd = QPointF(0.0, 0.0);
    }

    GraphicLine::~GraphicLine() {

    }

    GraphicLine::GraphicLine(const QPointF &start, const QPointF &end)
            : mStart(start), mEnd(end) {

    }

    GraphicLine &GraphicLine::operator=(const GraphicLine &line) {
        mStart = line.mStart;
        mEnd = line.mEnd;

        return *this;
    }

    const QPointF &GraphicLine::GetStart() const {
        return mStart;
    }

    const QPointF &GraphicLine::GetEnd() const {
        return mEnd;
    }

}
