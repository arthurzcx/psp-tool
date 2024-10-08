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
#include "base_def.h"
#include "base_graphic.h"

namespace PspNsBase {

    class BASE_SHARED GraphicLine : public Graphic {
    public:
        GraphicLine();
        virtual ~GraphicLine();

        GraphicLine(const QPointF &start, const QPointF &end);
        GraphicLine(const GraphicLine &line);

        GraphicLine &operator=(const GraphicLine &line);

        const QPointF &GetStart() const;
        const QPointF &GetEnd() const;

    private:
        QPointF mStart;
        QPointF mEnd;
    };

}
