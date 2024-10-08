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
#include "base/base_graphic_colorbar.h"

namespace PspNsBase {
    GraphicColorBar::GraphicColorBar()
            : mColorMin(0.0), mColorMax(255.0) {

    }

    GraphicColorBar::~GraphicColorBar() {

    }

    GraphicColorBar *GraphicColorBar::GetInstance() {
        static GraphicColorBar graphicColorBar;
        return &graphicColorBar;
    }

    GraphicColorBar::GraphicColorBar(double minColor, double maxColor)
            : mColorMin(minColor), mColorMax(maxColor) {

    }

    GraphicColorBar::GraphicColorBar(const GraphicColorBar &graphicColorBar)
            : mColorMin(graphicColorBar.mColorMin), mColorMax(graphicColorBar.mColorMax) {

    }

    GraphicColorBar &GraphicColorBar::operator=(const GraphicColorBar &graphicColorBar) {
        mColorMin = graphicColorBar.mColorMin;
        mColorMax = graphicColorBar.mColorMax;

        return *this;
    }

    double GraphicColorBar::GetColorMin() const {
        return mColorMin;
    }

    double GraphicColorBar::GetColorMax() const {
        return mColorMax;
    }

    void GraphicColorBar::SetColorMin(double colorMin) {
        mColorMin = colorMin;
    }

    void GraphicColorBar::SetColorMax(double colorMax) {
        mColorMax = colorMax;
    }

}
