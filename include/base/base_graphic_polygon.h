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
#include "base_graphic_rect.h"

#include <vector>

namespace PspNsBase {
    class BASE_SHARED GraphicPolygon : public Graphic {
    public:
        GraphicPolygon();
        virtual ~GraphicPolygon();

        GraphicPolygon(const std::vector<QPointF> &points);
        GraphicPolygon(const GraphicPolygon &polygon);
        GraphicPolygon &operator=(const GraphicPolygon &polygon);

        void AddPoint(const QPointF &point);
        const std::vector<QPointF> &GetPoints() const;
        void RemovePoint(int index);

        void MakeConvex();
        bool IsValid();

        bool IsInRect(const GraphicRect &rect);

    private:
        std::vector<QPointF> mPoints;
    };

    BASE_SHARED void to_json(Json &j, const PspNsBase::GraphicPolygon &polygon);
    BASE_SHARED void from_json(const Json &j, PspNsBase::GraphicPolygon &polygon);
}
