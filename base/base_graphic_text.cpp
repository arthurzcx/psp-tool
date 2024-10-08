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
#include "base/base_graphic_text.h"

namespace PspNsBase {
    GraphicText::GraphicText()
            : mPosition(QPointF(0, 0)) {

    }

    GraphicText::~GraphicText() {

    }

    GraphicText::GraphicText(const QPointF &position, const QStringList &text)
            : mPosition(position), mText(text) {

    }
    GraphicText::GraphicText(qreal x, qreal y, const QStringList &text)
            : mPosition(QPointF(x, y)), mText(text) {

    }

    GraphicText::GraphicText(const GraphicText &graphicText)
            : mPosition(graphicText.mPosition), mText(graphicText.mText) {

    }

    GraphicText &GraphicText::operator=(const GraphicText &graphicText) {
        mPosition = graphicText.mPosition;
        mText = graphicText.mText;

        return *this;
    }

    const QPointF &GraphicText::GetPosition() const {
        return mPosition;
    }

    const QStringList &GraphicText::GetText() const {
        return mText;
    }

    void GraphicText::SetPosition(const QPointF &position) {
        mPosition = position;
    }

    void GraphicText::SetText(const QStringList &text) {
        mText = text;
    }

    void GraphicText::AddText(const QString &text) {
        mText.append(text);
    }
}
