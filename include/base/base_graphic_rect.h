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
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace PspNsBase {
    class BASE_SHARED GraphicRect : public Graphic {
    public:
        GraphicRect();
        virtual ~GraphicRect();

        GraphicRect(const QPointF &leftTop, const QPointF &rightBottom);
        GraphicRect(qreal x, qreal y, qreal width, qreal height);

        GraphicRect(const GraphicRect &rectf);

        GraphicRect &operator=(const GraphicRect &rectf);

        const QRectF &GetRect() const;
        qreal GetLeft() const;
        qreal GetTop() const;
        QPointF GetLeftTopPoint() const;

        bool ContainPoint(const QPointF &point) const;

        static GraphicRect CreateRectFromCenter(const QPointF &center, qreal radius);
        GraphicRect ExpandByRaius(qreal halfRadius, qreal limitMaxX, qreal limitMaxY);

        bool IsValid(const GraphicRect &otherRect) const;

    private:
        QRectF mRectf;
    };

    BASE_SHARED void to_json(Json &j, const PspNsBase::GraphicRect &rect);
    BASE_SHARED void from_json(const Json &j, PspNsBase::GraphicRect &rect);
}
