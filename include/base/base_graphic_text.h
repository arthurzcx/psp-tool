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

#include "base_global.h"
#include "base_def.h"
#include "base_graphic.h"
#include <QStringList>
#include <QString>

namespace PspNsBase {

    class BASE_SHARED GraphicText : public Graphic {
    public:
        GraphicText();
        virtual ~GraphicText();

        GraphicText(const QPointF &position, const QStringList &text);
        GraphicText(qreal x, qreal y, const QStringList &text);

        GraphicText(const GraphicText &graphicText);

        GraphicText &operator=(const GraphicText &graphicText);

        const QPointF &GetPosition() const;
        const QStringList &GetText() const;

        void SetPosition(const QPointF &position);
        void SetText(const QStringList &text);

        void AddText(const QString &text);

    private:
        QPointF mPosition;
        QStringList mText;
    };

}
