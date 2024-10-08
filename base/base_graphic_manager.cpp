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
#include "base/base_graphic_manager.h"

namespace PspNsBase {
    GraphicManager::GraphicManager() {}


    GraphicManager::~GraphicManager() {
        DestroyAllGraphics();
    }

    void GraphicManager::AddGraphic(Graphic *graphic) {
        mGraphics.push_back(graphic);
    }

    const std::vector<Graphic *> &GraphicManager::GetGraphics() const {
        return mGraphics;
    }

    void GraphicManager::DestroyAllGraphics() {
        for (auto graphic: mGraphics) {
            if (graphic) {
                delete graphic;
                graphic = nullptr;
            }
        }
        mGraphics.clear();
    }

    void GraphicManager::Clear() {
        mGraphics.clear();
    }
}
