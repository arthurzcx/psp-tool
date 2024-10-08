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
#include "base/base_graphic_rect.h"
#include <cmath>

namespace PspNsBase {
    GraphicRect::GraphicRect()
            : Graphic(), mRectf(QPointF(MAX_VALUE, MAX_VALUE), QPointF(MAX_VALUE, MAX_VALUE)) {

    }

    GraphicRect::~GraphicRect() {

    }

    GraphicRect::GraphicRect(const QPointF &start, const QPointF &end)
            : Graphic(), mRectf(start, end) {

    }

    GraphicRect::GraphicRect(qreal x, qreal y, qreal width, qreal height)
            : Graphic(), mRectf(x, y, width, height) {

    }

    GraphicRect::GraphicRect(const GraphicRect &rectf)
            : Graphic(rectf), mRectf(rectf.mRectf) {

    }

    GraphicRect &GraphicRect::operator=(const GraphicRect &rectf) {
        mRectf = rectf.mRectf;

        return *this;
    }

    const QRectF &GraphicRect::GetRect() const {
        return mRectf;
    }

    qreal GraphicRect::GetLeft() const {
        return mRectf.left();
    }

    qreal GraphicRect::GetTop() const {
        return mRectf.top();
    }

    QPointF GraphicRect::GetLeftTopPoint() const {
        return QPointF(mRectf.left(), mRectf.top());
    }

    bool GraphicRect::ContainPoint(const QPointF &point) const {
        return (point.x() - mRectf.left() > EPSILON &&
                mRectf.right() - point.x() > EPSILON &&
                point.y() - fmin(mRectf.top(), mRectf.bottom()) > EPSILON &&
                fmax(mRectf.bottom(), mRectf.top()) - point.y() > EPSILON);
    }

    GraphicRect GraphicRect::CreateRectFromCenter(const QPointF &center, qreal radius) {
        qreal left = center.x() - radius;
        qreal right = center.x() + radius;
        qreal top = center.y() - radius;
        qreal bottom = center.y() + radius;

        left = fmax(0, left);
        top = fmax(0, top);

        return GraphicRect(QPointF(left, top), QPointF(right, bottom));
    }

    GraphicRect GraphicRect::ExpandByRaius(qreal halfRadius, qreal limitMaxX, qreal limitMaxY) {
        qreal left = fmax(mRectf.left() - halfRadius, 0);
        qreal right = fmin(mRectf.right() + halfRadius, limitMaxX - 1);
        qreal top = fmax(mRectf.top() - halfRadius, 0);
        qreal bottom = fmin(mRectf.bottom() + halfRadius, limitMaxY - 1);

        return GraphicRect(QPointF(left, top), QPointF(right, bottom));
    }

    bool GraphicRect::IsValid(const GraphicRect &otherRect) const {
        if (mRectf.x() < 0 || mRectf.y() < 0 ||
            mRectf.width() <= 0 || mRectf.height() <= 0 ||
            mRectf.width() > otherRect.mRectf.width() ||
            mRectf.height() > otherRect.mRectf.height() ||
            mRectf.x() + mRectf.width() > otherRect.mRectf.width() ||
            mRectf.y() + mRectf.height() > otherRect.mRectf.height()) {
            return false;
        }

        return true;
    }

    BASE_SHARED void to_json(Json &j, const PspNsBase::GraphicRect &rect) {
        j["type"] = "rect";
        j["leftTopX"] = rect.GetLeftTopPoint().x();
        j["leftTopY"] = rect.GetLeftTopPoint().y();
        j["width"] = rect.GetRect().width();
        j["height"] = rect.GetRect().height();
    }

    BASE_SHARED void from_json(const Json &j, PspNsBase::GraphicRect &rect) {
        rect = GraphicRect(j["leftTopX"], j["leftTopY"], j["width"], j["height"]);
    }
}
